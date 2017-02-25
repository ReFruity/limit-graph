#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <cassert>
#include <set>

using namespace std;

struct Triple {
    int x, v, y;

    Triple() { }

    Triple(int x, int v, int y): x(x), v(v), y(y) { }

    string toString() {
        stringstream result;
        result << "(" << x << ", " << v << ", " << y << ")";
        return result.str();
    }
};

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

    void rotateEdge(Triple* triple) {
        this->rotateEdge(triple->x, triple->v, triple->y);
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

    bool isIncreasingTriple(Triple* triple) {
        return isIncreasingTriple(triple->x, triple->v, triple->y);
    }

    bool isIncreasingTriple(int x, int v, int y) {
        if (!areConnected(x, v) || areConnected(y, v)) {
            return false;
        }

        return deg(x) <= deg(y);
    }

    bool isDecreasingTriple(Triple* triple) {
        return isDecreasingTriple(triple->x, triple->v, triple->y);
    }

    bool isDecreasingTriple(int x, int v, int y) {
        if (!areConnected(x, v) || areConnected(y, v)) {
            return false;
        }

        return deg(x) > deg(y) + 1;
    }

    Triple* maxIncreasingTriplePtr() {
        vector<pair<int, int>> graphSequence;
        for (int x = 0; x < this->size(); x++) {
            graphSequence.push_back(pair<int, int>(x, this->deg(x)));
        }
        sort(graphSequence.begin(), graphSequence.end(), [](pair<int, int> p1, pair<int, int> p2){ return p1.second > p2.second; });

        for (int i = 0; i < graphSequence.size(); i++) {
            int y = graphSequence[i].first;

            for (int j = graphSequence.size() - 1; j > i; j--) {
                int x = graphSequence[j].first;

                for (int v = 0; v < this->size(); v++) {
                    if (x == v || y == v) {
                        continue;
                    }

                    if (this->areConnected(x, v) && !this->areConnected(y, v)) {
                        return new Triple(x, v, y);
                    }
                }
            }
        }

        return nullptr;
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
        return this->maxIncreasingTriplePtr() == nullptr;
    }

    int size() {
        return adjacencyMatrix.size();
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
};

ostream &operator<<(ostream &strm, Graph &graph) {
    return strm << graph.toString();
}

ostream &operator<<(ostream &strm, Triple &triple) {
    return strm << triple.toString();
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
}

void greedyEdgeRotation(Graph &graph) {
    int rotations = 0;
    Triple* triplePtr;
    cout << graph << endl;

    while((triplePtr = graph.maxIncreasingTriplePtr()) != nullptr) {
        graph.rotateEdge(triplePtr);
        rotations++;
        cout << "Rotation #" << rotations << " " << *triplePtr << endl;
        cout << graph << endl;
        delete triplePtr;
    }

    cout << "Total: " << rotations << " rotations." << endl;
}

Graph* randomGraphPtr(unsigned int size, unsigned int seed) {
    vector<vector<short>> adjacencyMatrix(size, vector<short>(size, 0));

    srand(seed);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < i; j++) {
            short randomNum = (short) (rand() % 2);
            adjacencyMatrix[i][j] = adjacencyMatrix[j][i] = randomNum;
        }
    }

    return new Graph(adjacencyMatrix);
}

int main() {
    test();

    //auto adjacencyMatrix = vector<vector<short>>({{0, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}, {0, 0, 1, 0}});
    //Graph graph = Graph(adjacencyMatrix);
    Graph graph = *randomGraphPtr(10, 0);

    greedyEdgeRotation(graph);

    return 0;
}
