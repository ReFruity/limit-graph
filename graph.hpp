#ifndef LIMIT_GRAPH_GRAPH_HPP
#define LIMIT_GRAPH_GRAPH_HPP

#include <string>
#include <vector>
#include <memory>
#include <sstream>
#include <iterator>
#include <algorithm>

using namespace std;

struct Triple {
    int x, v, y;

    Triple();

    Triple(int x, int v, int y);

    string toString() const;
};

class Graph {
private:
    vector<vector<short>> adjacencyMatrix;

public:
    Graph(vector<vector<short>> adjacencyMatrix);

    bool operator==(const Graph &other) const;

    bool operator!=(const Graph &other) const;

    void rotateEdge(const unique_ptr<Triple>& triplePtr);

    void rotateEdge(int x, int v, int y);

    bool isIncreasingTriple(Triple* triple);

    bool isIncreasingTriple(int x, int v, int y);

    bool isDecreasingTriple(Triple* triple);

    bool isDecreasingTriple(int x, int v, int y);

    unique_ptr<Triple> maxIncreasingTriplePtr();

    bool areConnected(int x, int y);

    int deg(int x) const;

    void connect(int x, int y);

    void disconnect(int x, int y);

    bool isLimit();

    int size() const;

    string toString() const;
};

ostream &operator<<(ostream &strm, const Triple &triple);

ostream &operator<<(ostream &strm, const Graph &graph);

#endif //LIMIT_GRAPH_GRAPH_HPP
