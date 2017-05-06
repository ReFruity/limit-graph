#include <iostream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <sstream>
#include <cassert>

using namespace std;

// region Graph

struct Triple {
    int x, v, y;

    Triple() {}

    Triple(int x, int v, int y): x(x), v(v), y(y) {}

    string toString() const {
        stringstream result;
        result << "(" << x << ", " << v << ", " << y << ")";
        return result.str();
    }
};

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

    bool operator==(const Graph &other) const {
        return adjacencyMatrix == other.adjacencyMatrix;
    }

    bool operator!=(const Graph &other) const {
        return !(other == *this);
    }

    void rotateEdge(Triple* triple) {
        rotateEdge(triple->x, triple->v, triple->y);
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
        for (int x = 0; x < size(); x++) {
            graphSequence.push_back(pair<int, int>(x, deg(x)));
        }
        sort(graphSequence.begin(),
             graphSequence.end(),
             [](pair<int, int> p1, pair<int, int> p2){ return p1.second > p2.second; });

        for (int i = 0; i < graphSequence.size(); i++) {
            int y = graphSequence[i].first;

            for (int j = graphSequence.size() - 1; j > i; j--) {
                int x = graphSequence[j].first;

                for (int v = 0; v < size(); v++) {
                    if (x == v || y == v) {
                        continue;
                    }

                    if (areConnected(x, v) && !areConnected(y, v)) {
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
        return maxIncreasingTriplePtr() == nullptr;
    }

    int size() {
        return adjacencyMatrix.size();
    }

    string toString() const {
        stringstream result;
        result << "{\n";

        for (auto &line: adjacencyMatrix) {
            stringstream lineStream;
            copy(line.begin(), line.end() - 1, ostream_iterator<short>(lineStream, ", "));
            lineStream << line[line.size() - 1];
            result << "{" << lineStream.str().c_str() << "},\n";
        }

        result << "}\n";
        return result.str();
    }
};

// endregion

// region Partition

class Partition {
private:
    unsigned int num;
    vector<unsigned int> content;

public:
    Partition(vector<unsigned int> content) :
            num(accumulate(content.begin(), content.end(), 0u)),
            content(content)
    {}

    Partition(const Partition& other) {
        num = other.num;
        content = std::vector<unsigned int>(other.content);
    }

    static Partition from(int columns, int rows) {
        return Partition(vector<unsigned int>(columns, rows));
    }

    void move(int from, int to) {
        if (to >= content.size()) {
            content.resize((unsigned int) to + 1);
        }

        content[from]--;
        content[to]++;
    }

    void insert(int columnIndex) {
        if (columnIndex >= content.size()) {
            content.resize((unsigned int) columnIndex + 1);
        }

        content[columnIndex]++;
        num++;
    }

    void remove(int columnIndex) {
        content[columnIndex]--;
        num--;
    }

    void fillHead() {
        int halfDelta = (tail().sum() - head().sum())/2;
        int thisRank = rank();

        for (int rowIndex = thisRank; rowIndex < 2*thisRank; rowIndex++) {
            if (halfDelta == 0) {
                break;
            }

            for (int columnIndex = 0; columnIndex < thisRank; columnIndex++) {
                if (content[columnIndex] <= rowIndex) {
                    insert(columnIndex);
                    halfDelta--;
                }

                if (halfDelta == 0) {
                    break;
                }
            }
        }
    }

    void maximize() {
        fillHead();
        Partition conjugateHead = head().conjugate();
        int thisRank = rank();
        int thisLength = length();

        for (int i = thisRank; i < thisLength; i++) {
            content[i] = conjugateHead[i - thisRank];
        }

        num = conjugateHead.sum() * 2 + (thisRank - 1) * thisRank;
    }

    bool isValid() {
        if (num != accumulate(content.begin(), content.end(), 0u)) {
            return false;
        }

        if (!is_sorted(content.begin(), content.end(), greater<unsigned int>())) {
            return false;
        }

        return true;
    }

    bool isMaximumGraphical() {
        return head() == tail();
    }

    bool isGraphical() {
        return sum() % 2 == 0 && tail() >= head();
    }

    unsigned int sum() {
        return num;
    }

    unsigned int length() const {
        for (unsigned int i = 0; i < content.size(); i++) {
            if (content[i] == 0) {
                return i;
            }
        }

        return content.size();
    }

    unsigned int rank() {
        for (unsigned int i = 0; i < content.size(); i++) {
            if (content[i] <= i) {
                return i;
            }
        }

        return content.size();
    }

    int rightmost(int index) {
        if ((*this)[index] == 0) {
            return index;
        }

        unsigned int start = (*this)[index];

        for (index++; index < content.size() + 1; index++) {
            if ((*this)[index] < start) {
                return index - 1;
            }
        }

        stringstream errorMessage;
        errorMessage << "Partition '" << toString() << "' has invalid state.";
        throw domain_error(errorMessage.str());
    }

    Partition head() {
        int thisRank = rank();

        auto resultContent = vector<unsigned int>(content.begin(), content.begin() + thisRank);
        for_each(resultContent.begin(), resultContent.end(), [thisRank](unsigned int& val){ val -= (thisRank - 1); });

        return Partition(resultContent);
    }

    Partition tail() {
        return Partition(vector<unsigned int>(content.begin() + rank(), content.end())).conjugate();
    }

    Partition conjugate() {
        auto resultContent = vector<unsigned int>();

        for (int i = 0; i < content[0]; i++) {
            resultContent.push_back(0);

            for (int j = 0; j < content.size(); j++) {
                if (content[j] > i) {
                    resultContent[i]++;
                }

                if (content[j] == 0) {
                    break;
                }
            }
        }

        return Partition(resultContent);
    }

    bool operator==(const Partition& other) const {
        if (num != other.num) {
            return false;
        }

        for (int i = 0; i < min(content.size(), other.content.size()); i++) {
            if (content[i] != other.content[i]) {
                return false;
            }

            if (content[i] == other.content[i] == 0) {
                break;
            }
        }

        return true;
    }

    bool operator!=(const Partition& other) const {
        return !(*this == other);
    }

    bool operator<=(const Partition& other) const {
        return other >= *this;
    }

    bool operator>=(const Partition& other) const {
        int thisPartialSum = 0;
        int otherPartialSum = 0;

        for (int i = 0; i < max(length(), other.length()); i++) {
            thisPartialSum += (*this)[i];
            otherPartialSum += other[i];

            if (thisPartialSum < otherPartialSum) {
                return false;
            }
        }

        return true;
    }

    bool operator<(const Partition& other) const {
        return (*this) <= other && (*this) != other;
    }

    bool operator>(const Partition& other) const {
        return (*this) >= other && (*this) != other;
    }

    unsigned int operator[](int index) const {
        if (index < content.size()) {
            return content[index];
        }

        return 0;
    }

    string toString() const {
        stringstream stringStream;

        if (length() == 0) {
            stringStream << "0 ";
        }
        else {
            copy(content.begin(), content.begin() + length(), ostream_iterator<unsigned int>(stringStream, " "));
        }
        stringStream << "| " << num;

        return stringStream.str();
    }
};

class PartitionTransition {
public:
    virtual PartitionTransition* copy() = 0;

    virtual void apply(Partition& partition) = 0;

    virtual PartitionTransition* inverse() = 0;

    virtual bool isAscending() = 0;

    virtual bool isDescending() = 0;

    virtual bool isIdentical() = 0;
    
    virtual bool isMove() = 0;

    virtual bool isInsert() = 0;

    virtual bool isRemove() = 0;
    
    virtual bool operator==(const PartitionTransition& other) = 0;

    bool operator!=(const PartitionTransition& other) {
        return !((*this) == other);
    };

    virtual string toString() const = 0;
};

class PartitionMove : public PartitionTransition {
private:
    int from, to;

public:
    PartitionMove(int from, int to) {
        this->from = from;
        this->to = to;
    }

    PartitionTransition* copy() {
        return new PartitionMove(from, to);
    }

    void apply(Partition& partition) {
        partition.move(from, to);
    }

    PartitionTransition* inverse() {
        return new PartitionMove(to, from);
    }

    bool isAscending() {
        return from < to;
    }

    bool isDescending() {
        return from > to;
    }

    bool isIdentical() {
        return from == to;
    }

    bool isMove() {
        return true;
    }

    bool isInsert() {
        return false;
    }

    bool isRemove() {
        return false;
    }

    bool operator==(const PartitionTransition& other) {
        const PartitionMove* otherPtr = dynamic_cast<const PartitionMove*>(&other);

        if (otherPtr == nullptr) {
            return false;
        }

        return otherPtr->from == from && otherPtr->to == to;
    }

    string toString() const {
        stringstream result;
        result << "(" << from << "->" << to << ")";
        return result.str();
    }
};

// TODO: Extract header
class PartitionRemove : public PartitionTransition {
private:
    int columnIndex;

public:
    PartitionRemove(int);

    PartitionTransition* copy();

    void apply(Partition&);

    PartitionTransition* inverse();

    bool isAscending();

    bool isDescending();

    bool isIdentical();

    bool isMove();

    bool isInsert();

    bool isRemove();

    bool operator==(const PartitionTransition&);

    string toString() const;
};

class PartitionInsert : public PartitionTransition {
private:
    int columnIndex;

public:
    PartitionInsert(int columnIndex) {
        this->columnIndex = columnIndex;
    }

    PartitionTransition* copy() {
        return new PartitionInsert(columnIndex);
    }

    void apply(Partition& partition) {
        partition.insert(columnIndex);
    }

    PartitionTransition* inverse() {
        return new PartitionRemove(columnIndex);
    }

    bool isAscending() {
        return true;
    }

    bool isDescending() {
        return false;
    }

    bool isIdentical() {
        return false;
    }

    bool isMove() {
        return false;
    }

    bool isInsert() {
        return true;
    }

    bool isRemove() {
        return false;
    }

    bool operator==(const PartitionTransition& other) {
        const PartitionInsert* otherPtr = dynamic_cast<const PartitionInsert*>(&other);

        if (otherPtr == nullptr) {
            return false;
        }

        return otherPtr->columnIndex == columnIndex;
    }

    string toString() const {
        stringstream result;
        result << "(" << "+" << columnIndex << ")";
        return result.str();
    }
};

PartitionRemove::PartitionRemove(int columnIndex) {
    this->columnIndex = columnIndex;
}

PartitionTransition *PartitionRemove::copy() {
    return new PartitionRemove(columnIndex);
}

// region PartitionRemove implementation

void PartitionRemove::apply(Partition& partition) {
    partition.remove(columnIndex);
}

PartitionTransition* PartitionRemove::inverse() {
    return new PartitionInsert(columnIndex);
}

bool PartitionRemove::isAscending() {
    return false;
}

bool PartitionRemove::isDescending() {
    return true;
}

bool PartitionRemove::isIdentical() {
    return false;
}

bool PartitionRemove::isMove() {
    return false;
}

bool PartitionRemove::isInsert() {
    return false;
}

bool PartitionRemove::isRemove() {
    return true;
}

bool PartitionRemove::operator==(const PartitionTransition& other) {
    const PartitionRemove* otherPtr = dynamic_cast<const PartitionRemove*>(&other);

    if (otherPtr == nullptr) {
        return false;
    }

    return otherPtr->columnIndex == columnIndex;
}

string PartitionRemove::toString() const {
    stringstream result;
    result << "(" << "-" << columnIndex << ")";
    return result.str();
}

// endregion

class TransitionChain {
private:
    vector<PartitionTransition*> transitionPtrs;

    void copyFrom(vector<PartitionTransition*> transitionPtrs) {
        this->transitionPtrs = vector<PartitionTransition*>(transitionPtrs.capacity());

        transform(
                transitionPtrs.begin(),
                transitionPtrs.end(),
                this->transitionPtrs.begin(),
                [](PartitionTransition* tptr){ return tptr->copy(); }
        );
    }

public:
    TransitionChain() {}

    TransitionChain(const TransitionChain& other) {
        copyFrom(other.transitionPtrs);
    }

    TransitionChain(vector<PartitionTransition*> transitionPtrs) {
        copyFrom(transitionPtrs);
    }

    TransitionChain inverse() {
        vector<PartitionTransition*> resultTransitionPtrs(transitionPtrs.capacity());

        transform(
                transitionPtrs.rbegin(),
                transitionPtrs.rend(),
                resultTransitionPtrs.begin(),
                [](PartitionTransition* tptr){ return tptr->inverse(); }
        );

        return TransitionChain(resultTransitionPtrs);
    }

    void push_back(PartitionTransition* transitionPtr) {
        transitionPtrs.push_back(transitionPtr);
    }

    void apply(Partition& partition) {
        for (auto it = transitionPtrs.begin(); it != transitionPtrs.end(); it++) {
            (*it)->apply(partition);
        }
    }

    int length() {
        return transitionPtrs.size();
    }

    PartitionTransition* operator[](int index) {
        return transitionPtrs[index];
    }

    TransitionChain& operator=(const TransitionChain& other) {
        copyFrom(other.transitionPtrs);
        return *this;
    }

    bool operator==(const TransitionChain& other) {
        if (transitionPtrs.size() != other.transitionPtrs.size()) {
            return false;
        }

        for (int i = 0; i < transitionPtrs.size(); i++) {
            if (*(transitionPtrs[i]) != *(other.transitionPtrs[i])) {
                return false;
            }
        }

        return true;
    }

    string toString() const {
        stringstream result;
        result << "[";

        if (!transitionPtrs.empty()) {
            auto it = transitionPtrs.begin();
            for (; it != prev(transitionPtrs.end()); it++) {
                result << (*it)->toString();
                result << "=>";
            }

            result << (*it)->toString();
        }

        result << "]";

        return result.str();
    }

    ~TransitionChain() {
        for (auto it = transitionPtrs.begin(); it != transitionPtrs.end(); it++) {
            delete *it;
        }
    }
};

// endregion

// region Output and predeclaration

ostream &operator<<(ostream &strm, const Graph &graph) {
    return strm << graph.toString();
}

ostream &operator<<(ostream &strm, const Triple &triple) {
    return strm << triple.toString();
}

ostream &operator<<(ostream &strm, const Partition &partition) {
    return strm << partition.toString();
}

ostream &operator<<(ostream &strm, const PartitionTransition &transition) {
    return strm << transition.toString();
}

ostream &operator<<(ostream &strm, const TransitionChain &transitions) {
    return strm << transitions.toString();
}

TransitionChain graphicallyMaximizingChain(Partition partition) ;

TransitionChain headTailConjugateChain(Partition partition) ;

TransitionChain partitionTransitionChain(Partition from, Partition to) ;

// endregion

// TODO: Fix memory leaks
void test() {
    // region Graph

    vector<vector<short>> adjacencyMatrix;

    adjacencyMatrix = vector<vector<short>>({{0, 1}, {1, 0}});
    Graph graph = Graph(adjacencyMatrix);
    assert(graph.isLimit());
    assert(graph == Graph(adjacencyMatrix));

    adjacencyMatrix = vector<vector<short>>({{0, 1, 0}, {1, 0, 1}, {0, 1, 0}});
    graph = Graph(adjacencyMatrix);
    assert(graph.isLimit());
    assert(graph == Graph(adjacencyMatrix));

    graph.rotateEdge(1, 2, 0);
    assert(graph == Graph({{0, 1, 1}, {1, 0, 0}, {1, 0, 0}}));

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

    // endregion

    // region Partition

    // region Methods

    assert(Partition({3, 2, 2, 1}).rank() == 2);
    assert(Partition({5, 5, 5, 5}).rank() == 4);
    assert(!Partition({1, 2, 3, 4, 5}).isValid());
    assert(Partition({7, 5, 5, 4, 1}).rightmost(0) == 0);
    assert(Partition({7, 5, 5, 4, 1}).rightmost(1) == 2);
    assert(Partition({7, 5, 5, 4, 1}).rightmost(2) == 2);
    assert(Partition({7, 5, 5, 4, 1}).rightmost(3) == 3);
    assert(Partition({7, 5, 5, 4, 1}).rightmost(4) == 4);
    assert(Partition({7, 5, 5, 4, 1}).rightmost(5) == 5);

    Partition partition = Partition({2, 1});
    partition.move(0, 2);

    assert(partition.isValid());
    assert(partition.length() == 3);
    assert(partition == Partition({1, 1, 1}));

    partition = Partition({2, 1});
    partition.insert(2);

    assert(partition.isValid());
    assert(partition.length() == 3);
    assert(partition == Partition({2, 1, 1}));

    partition = Partition({5, 3, 1, 1});

    assert(partition.rank() == 2);
    assert(partition.sum() == 10);
    assert(partition.isValid());
    assert(partition.head() == Partition({4, 2}));
    assert(partition.tail() == Partition({2}));

    partition.insert(5);
    partition.remove(5);
    partition.insert(10);
    partition.remove(10);

    partition.move(2, 3);
    assert(!partition.isValid());

    partition.move(3, 2);
    partition.move(1, 2);
    partition.remove(0);
    assert(partition.isValid());
    assert(!partition.isMaximumGraphical());

    partition.remove(0);
    assert(partition.isValid());
    assert(partition.isMaximumGraphical());

    assert(partition.head() == Partition({2, 1}));
    assert(partition.tail() == Partition({2, 1}));
    assert(partition.head() == partition.tail());

    partition.insert(1);
    assert(!partition.isMaximumGraphical());

    assert(partition.conjugate() == Partition({4, 3, 2}));

    assert(Partition({5, 4, 2, 2, 2, 1}).isGraphical());
    assert(Partition({5, 3, 2, 2, 2, 2}).isGraphical());
    assert(Partition::from(16, 1).isGraphical());
    assert(!(Partition({5, 5, 2, 2, 2}).isGraphical()));
    assert(!(Partition({6, 4, 2, 2, 1, 1}).isGraphical()));

    Partition notMaxPartition = Partition({4, 2, 2, 1, 1, 1, 1});

    partition = Partition(notMaxPartition);

    assert(partition.isGraphical());
    assert(partition.head() != partition.tail());

    partition.fillHead();

    assert(partition.isValid());
    assert(partition == Partition({4, 3, 2, 1, 1, 1, 1}));

    partition = Partition(notMaxPartition);
    partition.maximize();

    assert(partition.isValid());
    assert(partition.isMaximumGraphical());
    assert(partition == Partition({4, 3, 2, 2, 1}));
    assert(partition.sum() == notMaxPartition.sum());

    // endregion

    // region Comparison

    partition = Partition({5, 3, 1, 1});

    assert(partition <= Partition({10}));

    assert(partition >= Partition({4, 4, 1, 1}));
    assert(partition >= Partition({4, 3, 2, 1}));
    assert(partition >= Partition({4, 3, 1, 1, 1}));
    assert(partition >= Partition({3, 3, 2, 1, 1}));
    assert(partition >= Partition({3, 2, 2, 1, 1, 1}));
    assert(partition >= Partition::from(10, 1));

    assert(partition >= Partition({4, 3, 1, 1}));
    assert(partition >= Partition({5, 2, 1, 1}));
    assert(partition >= Partition({5, 3, 1}));
    assert(partition >= Partition({5, 2}));
    assert(partition >= Partition({}));

    assert(!(partition >= Partition({5, 4, 1})));
    assert(!(partition >= Partition({6, 2, 1, 1})));
    assert(!(partition >= Partition({5, 3, 1, 1, 1})));
    assert(!(partition >= Partition({6, 3, 1, 1})));
    assert(!(partition >= Partition({7, 1})));

    partition = Partition({4, 2, 1, 1});

    assert(partition >= partition);
    assert(partition >= Partition({1}));
    assert(partition >= Partition({3, 1}));
    assert(partition >= Partition({2, 2, 1, 1}));
    assert(partition >= Partition({4}));
    assert(partition >= Partition({4, 1}));
    assert(partition >= Partition({4, 2}));
    assert(partition >= Partition({4, 1, 1, 1, 1}));
    assert(partition >= Partition({3, 3, 1, 1}));
    assert(partition >= Partition({2, 2, 2, 2}));
    assert(partition >= Partition({2, 1, 1, 1, 1, 1, 1}));
    assert(partition >= Partition::from(8, 1));

    assert(partition <= partition);
    assert(partition <= Partition({5, 1, 1, 1}));
    assert(partition <= Partition({4, 2, 2}));
    assert(partition <= Partition({4, 3, 1}));
    assert(partition <= Partition({5, 2, 1}));
    assert(partition <= Partition({4, 4}));
    assert(partition <= Partition({4, 5}));
    assert(partition <= Partition({5, 3}));
    assert(partition <= Partition({6, 1, 1}));
    assert(partition <= Partition({6, 2}));
    assert(partition <= Partition({7, 1}));
    assert(partition <= Partition({8}));

    assert(!(partition >= Partition({4, 3})));
    assert(!(partition >= Partition({3, 3, 2})));
    assert(!(partition >= Partition({3, 3, 2, 1})));
    assert(!(partition >= Partition({3, 3, 1, 1, 1})));
    assert(!(partition >= Partition({5})));
    assert(!(partition >= Partition({5, 1})));
    assert(!(partition >= Partition({5, 1, 1})));
    assert(!(partition >= Partition({5, 2})));
    assert(!(partition >= Partition({6})));
    assert(!(partition >= Partition({6, 1})));
    assert(!(partition >= Partition({7})));

    assert(!(partition <= Partition({4, 3})));
    assert(!(partition <= Partition({3, 3, 2})));
    assert(!(partition <= Partition({3, 3, 2, 1})));
    assert(!(partition <= Partition({3, 3, 1, 1, 1})));
    assert(!(partition <= Partition({5})));
    assert(!(partition <= Partition({5, 1})));
    assert(!(partition <= Partition({5, 1, 1})));
    assert(!(partition <= Partition({5, 2})));
    assert(!(partition <= Partition({6})));
    assert(!(partition <= Partition({6, 1})));
    assert(!(partition <= Partition({7})));

    // endregion

    // region Transition

    PartitionTransition* transitionPtr = nullptr;

    partition = Partition({2, 1});
    transitionPtr = new PartitionMove(0, 2);
    transitionPtr->apply(partition);
    
    assert(partition.isValid());
    assert(partition.length() == 3);
    assert(partition == Partition({1, 1, 1}));

    partition = Partition({2, 1});
    transitionPtr = new PartitionInsert(1);
    transitionPtr->apply(partition);

    assert(partition.isValid());
    assert(partition.length() == 2);
    assert(partition == Partition({2, 2}));

    partition = Partition({2, 1});
    transitionPtr = new PartitionInsert(2);
    transitionPtr->apply(partition);

    assert(partition.isValid());
    assert(partition.length() == 3);
    assert(partition == Partition({2, 1, 1}));

    partition = Partition({2, 1});
    transitionPtr = new PartitionRemove(0);
    transitionPtr->apply(partition);

    assert(partition.isValid());
    assert(partition.length() == 2);
    assert(partition == Partition({1, 1}));

    partition = Partition({2, 1});
    transitionPtr = new PartitionRemove(1);
    transitionPtr->apply(partition);

    assert(partition.isValid());
    assert(partition.length() == 1);
    assert(partition == Partition({2}));

    transitionPtr = new PartitionMove(0, 1);

    assert(*(transitionPtr->inverse()) == PartitionMove(1, 0));
    assert(*(transitionPtr->inverse()) != PartitionMove(0, 1));
    assert(*(transitionPtr->inverse()) != PartitionInsert(0));
    assert(*(transitionPtr->inverse()) != PartitionRemove(0));

    transitionPtr = new PartitionInsert(0);

    assert(*(transitionPtr->inverse()) == PartitionRemove(0));
    assert(*(transitionPtr->inverse()) != PartitionMove(0, 1));
    assert(*(transitionPtr->inverse()) != PartitionInsert(0));
    assert(*(transitionPtr->inverse()) != PartitionRemove(1));

    transitionPtr = new PartitionRemove(0);

    assert(*(transitionPtr->inverse()) == PartitionInsert(0));
    assert(*(transitionPtr->inverse()) != PartitionMove(0, 1));
    assert(*(transitionPtr->inverse()) != PartitionInsert(1));
    assert(*(transitionPtr->inverse()) != PartitionRemove(0));

    TransitionChain chain = TransitionChain({
            new PartitionMove(0, 1),
            new PartitionInsert(0),
            new PartitionRemove(0)
    });

    assert(chain == TransitionChain(chain));

    TransitionChain expectedChain = TransitionChain({
            new PartitionInsert(0),
            new PartitionRemove(0),
            new PartitionMove(1, 0)
    });

    assert(chain.inverse() == expectedChain);

    // endregion

    // region Algorithm

    auto actualChain = partitionTransitionChain(Partition({4, 4, 3}), Partition({6, 4, 1}));
    expectedChain = TransitionChain({new PartitionMove(2, 0), new PartitionMove(2, 0)});

    assert(actualChain == expectedChain);

    actualChain = partitionTransitionChain(Partition({2, 1, 1}), Partition({4}));
    expectedChain = TransitionChain({new PartitionMove(2, 0), new PartitionMove(1, 0)});

    assert(actualChain == expectedChain);

    actualChain = partitionTransitionChain(Partition({3, 1}), Partition({5, 1}));
    expectedChain = TransitionChain({new PartitionInsert(0), new PartitionInsert(0)});

    assert(actualChain == expectedChain);

    actualChain = partitionTransitionChain(Partition({3, 1, 1}), Partition({5, 3}));
    expectedChain = TransitionChain({
            new PartitionMove(2, 0),
            new PartitionInsert(0), new PartitionInsert(1), new PartitionInsert(1)
    });

    assert(actualChain == expectedChain);

    actualChain = partitionTransitionChain(Partition({2, 2}), Partition({5, 1}));
    expectedChain = TransitionChain({
            new PartitionMove(2, 0),
            new PartitionInsert(0), new PartitionInsert(1), new PartitionInsert(1)
    });

    assert(actualChain == expectedChain);

    partition = Partition({4, 2, 2, 1, 1, 1, 1});
    Partition headConjugate = partition.head().conjugate();
    Partition tailConjugate = partition.tail().conjugate();
    headTailConjugateChain(partition).apply(headConjugate);

    cout << partition.head() << endl;
    cout << partition.tail() << endl;
    cout << partition.head().conjugate() << endl;
    cout << partition.tail().conjugate() << endl;
    cout << headTailConjugateChain(partition) << endl;
    cout << headConjugate << endl;
    cout << tailConjugate << endl;

    assert(headConjugate == tailConjugate);

    partition = Partition({3, 3, 2, 1, 1, 1, 1});
    headConjugate = partition.head().conjugate();
    tailConjugate = partition.tail().conjugate();
    headTailConjugateChain(partition).apply(headConjugate);

    assert(headConjugate == tailConjugate);

    actualChain = graphicallyMaximizingChain(Partition({4, 2, 2, 1, 1, 1, 1}));

    expectedChain = TransitionChain({
            new PartitionMove(6, 1),
            new PartitionMove(5, 3)
    });

    assert(actualChain == expectedChain);

    // endregion

    // endregion

    cout << "Tests passed" << endl;
}

// region Algorithm

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

TransitionChain partitionTransitionChain(Partition from, Partition to) {
    if (!(from < to)) {
        throw invalid_argument("Arguments must satisfy: from < to");
    }

    TransitionChain result;
    int maxLength = max(from.length(), to.length());
    int searchIndex = 0;

    for (int i = 0; i < maxLength; i++) {
        if (from[i] <= to[i]) {
            continue;
        }

        for (int j = searchIndex; j < maxLength; j++) {
            if (from[j] >= to[j]) {
                continue;
            }

            int fromRightmost = from.rightmost(i);
            from.move(fromRightmost, j);
            assert(from.isValid());
            result.push_back(new PartitionMove(fromRightmost, j));
            if (from[j] == to[j]) {
                searchIndex = j + 1;
            }
            i--;

            break;
        }
    }

    // TODO
    // assert from[0:searchIndex] = to[0:searchIndex]
    // assert never from[i] > to[i]

    for (int i = searchIndex; i < maxLength; i++) {
        if (from[i] >= to[i]) {
            continue;
        }

        for (int j = to[i] - from[i]; j > 0; j--) {
            result.push_back(new PartitionInsert(i));
        }
    }

    return result;
}

TransitionChain headTailConjugateChain(Partition partition) {
    TransitionChain headTailChain = partitionTransitionChain(partition.head(), partition.tail());

    TransitionChain result;



    return partitionTransitionChain(partition.head().conjugate(), partition.tail().conjugate());
}

TransitionChain graphicallyMaximizingChain(Partition partition) {
    TransitionChain mainChain = headTailConjugateChain(partition);

    Partition maximumPartition = Partition(partition);
    maximumPartition.maximize();

    TransitionChain result;

    for (int i = 0; i < mainChain.length(); i++) {
        if (mainChain[i]->isInsert()) {
        }

        if (mainChain[i]->isMove()) {
        }
    }

    return result;
}

// endregion

int main(int argc, char *argv[]) {
    test();

    Graph* graphPtr;

    if (argc == 3) {
        unsigned int size = (unsigned int) atoi(argv[1]);
        unsigned int seed = (unsigned int) atoi(argv[2]);
        cout << "Size: " << size << endl;
        cout << "Random seed: " << seed << endl;
        graphPtr = randomGraphPtr(size, seed);
    }
    else {
        graphPtr = new Graph(
            {{0, 1, 0, 0},
             {1, 0, 1, 0},
             {0, 1, 0, 1},
             {0, 0, 1, 0}}
        );
        //graphPtr = randomGraphPtr(100, 0);
    }

    greedyEdgeRotation(*graphPtr);

    return 0;
}
