#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <cassert>
#include <set>
#include <climits>

using namespace std;

class Graph {
private:
    vector<vector<short>> adjacencyMatrix;

public:
    Graph(vector<vector<short>>& adjacencyMatrix) {
        if (adjacencyMatrix.size() != adjacencyMatrix[0].size()) {
            throw invalid_argument("Adjacency matrix should be square.");
        }

        this->adjacencyMatrix = adjacencyMatrix;
    }

    bool operator==(const Graph &other) const {
        return adjacencyMatrix == other.adjacencyMatrix;
    }

    bool operator!=(const Graph &other) const {
        return !(other == *this);
    }

    string toString() {
        stringstream result;
        result << "{\n";

        for(auto &line: adjacencyMatrix) {
            stringstream lineStream;
            copy(line.begin(), line.end() - 1, ostream_iterator<short>(lineStream, ", "));
            lineStream << line[line.size() - 1];
            result << "{" << lineStream.str().c_str() << "},\n";
        }

        result << "}\n";
        return result.str();
    }

    void rotateEdge(int x, int v, int y) {
        if (!areConnected(x, v) || areConnected(y, v)) {
            stringstream message;
            message << "Cannot rotate edge " << x << " " << v << " " << y << " in graph\n" << toString().c_str();
            throw runtime_error(message.str());
        }

        disconnect(x, v);
        connect(v, y);
    }

    bool isIncreasingTriple(int x, int v, int y) {
        if (!areConnected(x, v) || areConnected(y, v)) {
            return false;
        }

        return deg(x) <= deg(y);
    }

    bool isDecreasingTriple(int x, int v, int y) {
        if (!areConnected(x, v) || areConnected(y, v)) {
            return false;
        }

        return deg(x) > deg(y) + 1;
    }

    bool areConnected(int x, int y) {
        return adjacencyMatrix[x][y] > 0;
    }

    int deg(int x) {
        vector<short> line = adjacencyMatrix[x];
        return accumulate(line.begin(), line.end(), 0);
    }

    void connect(int x, int y) {
        adjacencyMatrix[x][y] = adjacencyMatrix[y][x] = 1;
    }

    void disconnect(int x, int y) {
        adjacencyMatrix[x][y] = adjacencyMatrix[y][x] = 0;
    }

    bool isLimit() {
        for (int x = 0; x < adjacencyMatrix.size(); x++) {
            for (int v = 0; v < adjacencyMatrix.size(); v++) {
                if (v == x) {
                    continue;
                }

                for (int y = 0; y < adjacencyMatrix.size(); y++) {
                    if (y == x || y == v) {
                        continue;
                    }

                    if (isIncreasingTriple(x, v, y)) {
                        return false;
                    }
                }
            }
        }

        return true;
    }

    vector<short> operator[](int x) const {
        return adjacencyMatrix[x];
    }

    int size() {
        return adjacencyMatrix.size();
    }
};

ostream &operator<<(ostream &strm, Graph &graph) {
    return strm << graph.toString();
}

void test() {
    auto adjacencyMatrix = vector<vector<short>>({{0, 1}, {1, 0}});
    Graph graph = Graph(adjacencyMatrix);
    assert(graph.isLimit());
    assert(graph == Graph(adjacencyMatrix));

    adjacencyMatrix = vector<vector<short>>({{0, 1, 0}, {1, 0, 1}, {0, 1, 0}});
    graph = Graph(adjacencyMatrix);
    assert(graph.isLimit());
    assert(graph == Graph(adjacencyMatrix));

    graph.rotateEdge(1, 2, 0);
    auto expectedMatrix = vector<vector<short>>({{0, 1, 1}, {1, 0, 0}, {1, 0, 0}});
    assert(graph == Graph(expectedMatrix));

    adjacencyMatrix = vector<vector<short>>({{0, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}, {0, 0, 1, 0}});
    graph = Graph(adjacencyMatrix);
    assert(!graph.isLimit());
    assert(!graph.isDecreasingTriple(3, 2, 0));
    assert(graph.isIncreasingTriple(3, 2, 0));

    graph.rotateEdge(3, 2, 0);
    assert(graph.isLimit());
    assert(graph.isDecreasingTriple(0, 2, 3));
    assert(!graph.isIncreasingTriple(0, 2, 3));

    graph = Graph(adjacencyMatrix);
    graph.connect(0, 2);
    assert(graph.isLimit());

    assert(graph[1] == vector<short>({1, 0, 1, 0}));
}

void greedyMaxFirst(Graph &graph) {
    cout << graph << endl;

    while (!graph.isLimit()) {
        outer_loop:

        vector<pair<int, int>> graphSequence;
        for (int x = 0; x < graph.size(); x++) {
            graphSequence.push_back(pair<int, int>(x, graph.deg(x)));
        }
        sort(graphSequence.begin(), graphSequence.end(), [](pair<int, int> p1, pair<int, int> p2){ return p1.second > p2.second; });

        cout << "Graph sequence (index degree): ";
        for (auto pair: graphSequence) {
            cout << pair.first << " " << pair.second << ", ";
        }
        cout << endl;

        for (int i = 0; i < graphSequence.size(); i++) {
            int y = graphSequence[i].first;

            for (int j = graphSequence.size() - 1; j > i; j--) {
                int x = graphSequence[j].first;

                for (int v = 0; v < graph.size(); v++) {
                    if (x == v || y == v) {
                        continue;
                    }

                    cout << "Trying (" << x << ", " << v << ", " << y << ")" << endl;

                    // Выполнение условия гарантирует, что тройка (x, v, y) является повышающей
                    if (graph.areConnected(x, v) && !graph.areConnected(y, v)) {
                        graph.rotateEdge(x, v, y);
                        cout << "Rotating (" << x << ", " << v << ", " << y << ")" << endl;
                        cout << graph << endl;
                        goto outer_loop;
                    }
                }
            }
        }
    }
}

int main() {
    test();

    auto adjacencyMatrix = vector<vector<short>>({{0, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}, {0, 0, 1, 0}});
    Graph graph = Graph(adjacencyMatrix);

    greedyMaxFirst(graph);

    return 0;
}
