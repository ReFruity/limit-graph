#include "graph.hpp"

#include <memory>

// region Triple

Triple::Triple() {}

Triple::Triple(int x, int v, int y) : x(x), v(v), y(y) {}

string Triple::toString() const {
    stringstream result;
    result << "(" << x << ", " << v << ", " << y << ")";
    return result.str();
}

// endregion

// region Graph

Graph::Graph(vector<vector<short>> adjacencyMatrix) {
    if (adjacencyMatrix.size() != adjacencyMatrix[0].size()) {
        throw invalid_argument("Adjacency matrix should be square.");
    }

    this->adjacencyMatrix = adjacencyMatrix;
}

bool Graph::operator==(const Graph& other) const {
    return adjacencyMatrix == other.adjacencyMatrix;
}

bool Graph::operator!=(const Graph& other) const {
    return !(other == *this);
}

void Graph::rotateEdge(const unique_ptr<Triple>& triplePtr) {
    rotateEdge(triplePtr->x, triplePtr->v, triplePtr->y);
}

void Graph::rotateEdge(int x, int v, int y) {
    disconnect(x, v);
    connect(v, y);
}

bool Graph::isIncreasingTriple(Triple *triple) {
    return isIncreasingTriple(triple->x, triple->v, triple->y);
}

bool Graph::isIncreasingTriple(int x, int v, int y) {
    if (!areConnected(x, v) || areConnected(y, v)) {
        return false;
    }

    return deg(x) <= deg(y);
}

bool Graph::isDecreasingTriple(Triple *triple) {
    return isDecreasingTriple(triple->x, triple->v, triple->y);
}

bool Graph::isDecreasingTriple(int x, int v, int y) {
    if (!areConnected(x, v) || areConnected(y, v)) {
        return false;
    }

    return deg(x) > deg(y) + 1;
}

unique_ptr<Triple> Graph::maxIncreasingTriplePtr() {
    auto thisSize = size();
    vector<pair<int, int>> graphSequence;
    graphSequence.reserve(thisSize);

    for (int x = 0; x < thisSize; x++) {
        graphSequence.emplace_back(x, deg(x));
    }

    sort(graphSequence.begin(),
         graphSequence.end(),
         [](pair<int, int> p1, pair<int, int> p2){ return p1.second > p2.second; });

    for (int i = 0; i < graphSequence.size(); i++) {
        int y = graphSequence[i].first;

        for (int j = graphSequence.size() - 1; j > i; j--) {
            int x = graphSequence[j].first;

            for (int v = 0; v < thisSize; v++) {
                if (x == v || y == v) {
                    continue;
                }

                if (areConnected(x, v) && !areConnected(y, v)) {
                    return make_unique<Triple>(x, v, y);
                }
            }
        }
    }

    return nullptr;
}

bool Graph::areConnected(int x, int y) {
    return adjacencyMatrix[x][y] > 0;
}

int Graph::deg(int x) const {
    vector<short> line = adjacencyMatrix[x];
    return accumulate(line.begin(), line.end(), 0);
}

void Graph::connect(int x, int y) {
    adjacencyMatrix[x][y] = adjacencyMatrix[y][x] = 1;
}

void Graph::disconnect(int x, int y) {
    adjacencyMatrix[x][y] = adjacencyMatrix[y][x] = 0;
}

bool Graph::isLimit() {
    return maxIncreasingTriplePtr() == nullptr;
}

int Graph::size() const {
    return adjacencyMatrix.size();
}

string Graph::toString() const {
    stringstream result;
    result << "{" << endl;

    for (auto &line: adjacencyMatrix) {
        stringstream lineStream;
        copy(line.begin(), line.end() - 1, ostream_iterator<short>(lineStream, ", "));
        lineStream << line[line.size() - 1];
        result << "{" << lineStream.str().c_str() << "}," << endl;
    }

    result << "}";
    return result.str();
}

// endregion

// region Output

ostream &operator<<(ostream &strm, const Triple &triple) {
    return strm << triple.toString();
}

ostream &operator<<(ostream &strm, const Graph &graph) {
    return strm << graph.toString();
}

// endregion
