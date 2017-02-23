#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>

using namespace std;

class Graph {
private:
    vector<vector<short>> adjacencyMatrix;

public:
    Graph(vector<vector<short>> adjacencyMatrix) {
        if (adjacencyMatrix.size() != adjacencyMatrix[0].size()) {
            throw invalid_argument("Adjacency matrix should be square.");
        }

        this->adjacencyMatrix = adjacencyMatrix;
    }

    string toString() {
        stringbuf result;

        for(auto &line: adjacencyMatrix) {
            stringstream lineStream;
            copy(line.begin(), line.end(), ostream_iterator<short>(lineStream, " "));
            string lineString = lineStream.str();
            result.sputn(lineString.c_str(), lineString.size());
            result.sputc('\n');
        }

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

    bool isGraphLimit() {
        for (int x = 0; x < adjacencyMatrix.size(); x++) {

        }
    }
};

int main() {
    vector<vector<short>> adjacencyMatrix = {{0, 1}, {1, 0}};

    Graph graph = Graph(adjacencyMatrix);

    cout << graph.toString() << endl;

    return 0;
}