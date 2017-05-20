#ifndef LIMIT_GRAPH_PARTITION_HPP
#define LIMIT_GRAPH_PARTITION_HPP

#include "graph.hpp"

using namespace std;

class Partition {
protected:
    unsigned int num;
    vector<unsigned int> content;

public:
    Partition(const vector<unsigned int>& content) :
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

    static Partition from(const Graph& graph) {
        vector<unsigned int> vertexDegrees;

        for (int vertex = 0; vertex < graph.size(); vertex++) {
            unsigned int vertexDegree = (unsigned int)graph.deg(vertex);

            if (vertexDegree > 0) {
                vertexDegrees.push_back(vertexDegree);
            }
        }

        sort(vertexDegrees.begin(), vertexDegrees.end(), greater<unsigned int>());

        return Partition(vertexDegrees);
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

    void replaceTail(Partition& newTail) {
        unsigned int thisRank = rank();
        unsigned int thisLength = length();

        for (int i = thisRank; i < thisLength; i++) {
            num = num - content[i] + newTail[i - thisRank];
            content[i] = newTail[i - thisRank];
        }
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

    bool isMaximumGraphical() const {
        return head() == tail();
    }

    bool isGraphical() {
        return sum() % 2 == 0 && tail() >= head();
    }

    unsigned int sum() const {
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

    unsigned int rank() const {
        for (unsigned int i = 0; i < content.size(); i++) {
            if (content[i] <= i) {
                return i;
            }
        }

        return content.size();
    }

    int rightmostByRow(int rowIndex) const {
        int start = rowIndex + 1;

        for (int i = 0; i < content.size() + 1; i++) {
            if ((*this)[i] < start) {
                return i - 1;
            }
        }

        stringstream message;
        message << "Partition '" << toString() << "' has invalid state.";
        throw runtime_error(message.str());
    }

    int rightmostByColumn(int columnIndex) {
        if ((*this)[columnIndex] == 0) {
            return columnIndex;
        }

        return rightmostByRow((*this)[columnIndex] - 1);
    }

    Partition head() const {
        int thisRank = rank();

        auto resultContent = vector<unsigned int>(content.begin(), content.begin() + thisRank);
        for_each(resultContent.begin(), resultContent.end(), [thisRank](unsigned int& val){ val -= (thisRank - 1); });

        return Partition(resultContent);
    }

    Partition tail() const {
        return Partition(vector<unsigned int>(content.begin() + rank(), content.end())).conjugate();
    }

    Partition conjugate() {
        auto resultContent = vector<unsigned int>();

        for (int i = 0; i < (*this)[0]; i++) {
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

    unique_ptr<vector<Partition>> graphicalChildrenPtr() const {
        return unique_ptr<vector<Partition>>(new vector<Partition>({Partition({2, 1, 1})}));
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
        return index < content.size() ? content[index] : 0;
    }

    string toString() const {
        stringstream stringStream;
        stringStream << "[";

        unsigned int thisLength = length();

        if (thisLength == 0) {
            stringStream << "0";
        }
        else {
            copy(content.begin(), content.begin() + thisLength - 1, ostream_iterator<unsigned int>(stringStream, " "));
            stringStream << content[thisLength - 1];
        }

        stringStream << "] | " << num;

        return stringStream.str();
    }
};

enum Color {
    BLACK = 'B',
    GREY = 'G',
    NONE = 'N'
};

class ColoredPartition {
private:
    Partition partition;
    vector<vector<Color>> colors;

    void resizeColorsVertically() {
        for (int i = 0; i < colors.size(); i++) {
            colors[i].resize(partition[0], NONE);
        }
    }

    void resizeColors() {
        unsigned int partitionLength = partition.length();

        if (partitionLength > colors.size()) {
            colors.resize(partitionLength);

            resizeColorsVertically();
        }

        if (partition[0] > colors[0].size()) {
            resizeColorsVertically();
        }
    };

public:
    ColoredPartition(const vector<unsigned int> &content)
            : partition(content), colors(content.size(), vector<Color>(content[0], NONE))
    {}

    ColoredPartition(const Partition &other)
            : partition(other), colors(other.length(), vector<Color>(other[0], NONE))
    {}

    ColoredPartition(const ColoredPartition &other)
            : partition(other.partition), colors(other.colors)
    {}

    void move(int from, int to) {
        partition.move(from, to);
        resizeColors();
        colors[to][partition[to] - 1] = colors[from][partition[from]];
        colors[from][partition[from]] = NONE;
    }

    void insert(int columnIndex) {
        partition.insert(columnIndex);
        resizeColors();
    }

    void remove(int columnIndex) {
        partition.remove(columnIndex);
        colors[columnIndex][partition[columnIndex]] = NONE;
    }

    void paint(Color color, int columnIndex) {
        colors[columnIndex][partition[columnIndex] - 1] = color;
    }

    void paint(Color color, int columnIndex, int rowIndex) {
        colors[columnIndex][rowIndex] = color;
    }

    Color getColor(int columnIndex) const {
        if (columnIndex > colors.size()) {
            return NONE;
        }

        return colors[columnIndex][partition[columnIndex] - 1];
    }

    Color getColor(int columnIndex, int rowIndex) const {
        if (columnIndex > colors.size()) {
            return NONE;
        }

        if (rowIndex > partition[columnIndex] - 1) {
            return NONE;
        }

        return colors[columnIndex][rowIndex];
    }

    bool hasBlock(int columnIndex, int rowIndex) {
        return partition[columnIndex] > rowIndex;
    }

    void paintHeadBlack() {
        int thisRank = rank();

        for (int rowIndex = thisRank - 1; rowIndex < partition[0]; rowIndex++) {
            for (int columnIndex = 0; columnIndex < thisRank; columnIndex++) {
                if (hasBlock(columnIndex, rowIndex)) {
                    paint(BLACK, columnIndex, rowIndex);
                }
            }
        }
    }

    void fillHead() {
        int halfDelta = (partition.tail().sum() - partition.head().sum()) / 2;
        int thisRank = rank();

        for (int rowIndex = thisRank; rowIndex < 2*thisRank; rowIndex++) {
            if (halfDelta == 0) {
                break;
            }

            for (int columnIndex = 0; columnIndex < thisRank; columnIndex++) {
                if (!hasBlock(columnIndex, rowIndex)) {
                    insert(columnIndex);
                    paint(GREY, columnIndex);
                    halfDelta--;
                }

                if (halfDelta == 0) {
                    break;
                }
            }
        }
    }

    void maximize() {
        paintHeadBlack();
        fillHead();
        Partition conjugateHead = partition.head().conjugate();
        int thisRank = rank();
        int thisLength = length();

        partition.replaceTail(conjugateHead);

        for (int i = thisRank; i < thisLength; i++) {
            for (int j = 0; j < partition[i]; j++) {
                colors[i][j] = colors[j][i - 1];
            }
        }
    }

    bool isValid() {
        if (!partition.isValid()) {
            return false;
        }

        if (colors.size() < partition.length()) {
            return false;
        }

        for (int i = 0; i < colors.size(); i++) {
            if (colors[i].size() < partition[i]) {
                return false;
            }

            for (int j = 0; j < colors[i].size(); j++) {
                if (j < partition[i]
                    && colors[i][j] != BLACK
                    && colors[i][j] != GREY
                    && colors[i][j] != NONE) {
                    return false;
                }

                if (j >= partition[i] && colors[i][j] != NONE) {
                    return false;
                }
            }
        }

        return true;
    }

    unsigned int sum() {
        return partition.sum();
    }

    unsigned int length() {
        return partition.length();
    }

    unsigned int rank() {
        return partition.rank();
    }

    bool operator==(const ColoredPartition& other) const {
        if (partition != other.partition) {
            return false;
        }

        for (int i = 0; i < min(colors.size(), other.colors.size()); i++) {
            for (int j = 0; j < min(colors[i].size(), other.colors[i].size()); j++) {
                if (colors[i][j] != other.colors[i][j]) {
                    return false;
                }
            }
        }

        return true;
    }

    unsigned int operator[](int index) const {
        return partition[index];
    }

    string toShortString() const {
        stringstream stringStream;

        stringStream << partition.toString() << endl;

        for (int i = 0; i < partition.length(); i++) {
            stringStream << char(getColor(i)) << " ";
        }

        return stringStream.str();
    }

    string toString() const {
        stringstream stringStream;

        for (int i = partition[0] - 1; i >= 0; i--) {
            for (int j = 0; j < partition.rightmostByRow(i) + 1; j++) {
                stringStream << char(getColor(j, i)) << " ";
            }

            if (i > 0) {
                stringStream << endl;
            }
        }

        stringStream << "| " << partition.sum();

        return stringStream.str();
    }
};

class PartitionTransition {
public:
    virtual unique_ptr<PartitionTransition> copy() = 0;

    virtual void apply(Partition& partition) = 0;

    virtual void apply(ColoredPartition& partition) = 0;

    virtual unique_ptr<PartitionTransition> inverse() = 0;

    virtual unique_ptr<PartitionTransition> conjugate() = 0;

    virtual bool isAscending() = 0;

    virtual bool isDescending() = 0;

    virtual bool isIdentical() = 0;

    virtual bool isMove() const = 0;

    virtual bool isInsert() const = 0;

    virtual bool isRemove() const = 0;

    virtual int insertColumn() const = 0;

    virtual int insertRow() const = 0;

    virtual int removeColumn() const = 0;

    virtual int removeRow() const = 0;

    virtual bool operator==(const PartitionTransition& other) = 0;

    bool operator!=(const PartitionTransition& other) {
        return !((*this) == other);
    };

    virtual string toString() const = 0;
};

class PartitionMove : public PartitionTransition {
private:
    int fromColumn, fromRow, toColumn, toRow;

public:
    PartitionMove(int fromColumn, int fromRow, int toColumn, int toRow)
            : fromColumn(fromColumn), fromRow(fromRow), toColumn(toColumn), toRow(toRow)
    {}

    unique_ptr<PartitionTransition> copy() {
        return unique_ptr<PartitionTransition>(new PartitionMove(fromColumn, fromRow, toColumn, toRow));
    }

    void apply(Partition& partition) {
        partition.move(fromColumn, toColumn);
    }

    void apply(ColoredPartition& cpartition) {
        cpartition.move(fromColumn, toColumn);
    }

    unique_ptr<PartitionTransition> inverse() {
        return unique_ptr<PartitionTransition>(new PartitionMove(toColumn, toRow, fromColumn, fromRow));
    }

    unique_ptr<PartitionTransition> conjugate() {
        return unique_ptr<PartitionTransition>(new PartitionMove(fromRow, fromColumn, toRow, toColumn));
    }

    bool isAscending() {
        return fromColumn < toColumn;
    }

    bool isDescending() {
        return fromColumn > toColumn;
    }

    bool isIdentical() {
        return fromColumn == toColumn;
    }

    bool isMove() const {
        return true;
    }

    bool isInsert() const {
        return false;
    }

    bool isRemove() const {
        return false;
    }

    int insertColumn() const {
        return toColumn;
    }

    int insertRow() const {
        return toRow;
    }

    int removeColumn() const {
        return fromColumn;
    }

    int removeRow() const {
        return fromRow;
    }

    bool operator==(const PartitionTransition& other) {
        const PartitionMove* otherPtr = dynamic_cast<const PartitionMove*>(&other);

        if (otherPtr == nullptr) {
            return false;
        }

        return otherPtr->fromColumn == fromColumn
               && otherPtr->fromRow == fromRow
               && otherPtr->toColumn == toColumn
               && otherPtr->toRow == toRow
                ;
    }

    string toString() const {
        stringstream result;
        result << "(" << fromColumn << "," << fromRow << "->" << toColumn << "," << toRow << ")";
        return result.str();
    }
};

class PartitionRemove : public PartitionTransition {
private:
    int columnIndex, rowIndex;

public:
    PartitionRemove(int, int);

    unique_ptr<PartitionTransition> copy();

    void apply(Partition&);

    void apply(ColoredPartition&);

    unique_ptr<PartitionTransition> inverse();

    unique_ptr<PartitionTransition> conjugate();

    bool isAscending();

    bool isDescending();

    bool isIdentical();

    bool isMove() const;

    bool isInsert() const;

    bool isRemove() const;

    bool operator==(const PartitionTransition&);

    string toString() const;

    int insertColumn() const;

    int insertRow() const;

    int removeColumn() const;

    int removeRow() const;
};

class PartitionInsert : public PartitionTransition {
private:
    int columnIndex, rowIndex;

public:
    PartitionInsert(int columnIndex, int rowIndex)
            : columnIndex(columnIndex), rowIndex(rowIndex)
    {}

    unique_ptr<PartitionTransition> copy() {
        return unique_ptr<PartitionTransition>(new PartitionInsert(columnIndex, rowIndex));
    }

    void apply(Partition& partition) {
        partition.insert(columnIndex);
    }

    void apply(ColoredPartition& cpartition) {
        cpartition.insert(columnIndex);
    }

    unique_ptr<PartitionTransition> inverse() {
        return unique_ptr<PartitionTransition>(new PartitionRemove(columnIndex, rowIndex));
    }

    unique_ptr<PartitionTransition> conjugate() {
        return unique_ptr<PartitionTransition>(new PartitionInsert(rowIndex, columnIndex));
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

    bool isMove() const {
        return false;
    }

    bool isInsert() const {
        return true;
    }

    bool isRemove() const {
        return false;
    }

    int insertColumn() const {
        return columnIndex;
    }

    int insertRow() const {
        return rowIndex;
    }

    int removeColumn() const {
        stringstream message;
        message << "Call of PartitionInsert::removeColumn for " << toString() << ".";
        throw runtime_error(message.str());
    }

    int removeRow() const {
        stringstream message;
        message << "Call of PartitionInsert::removeRow for " << toString() << ".";
        throw runtime_error(message.str());
    }

    bool operator==(const PartitionTransition& other) {
        const PartitionInsert* otherPtr = dynamic_cast<const PartitionInsert*>(&other);

        if (otherPtr == nullptr) {
            return false;
        }

        return otherPtr->columnIndex == columnIndex && otherPtr->rowIndex == rowIndex;
    }

    string toString() const {
        stringstream result;
        result << "(" << "+" << columnIndex << "," << rowIndex << ")";
        return result.str();
    }
};

class TransitionChain {
private:
    vector<PartitionTransition*> transitionPtrs;

    void copyFrom(const vector<PartitionTransition*>& transitionPtrs) {
        this->transitionPtrs = vector<PartitionTransition*>(transitionPtrs.size());

        transform(
                transitionPtrs.begin(),
                transitionPtrs.end(),
                this->transitionPtrs.begin(),
                [](PartitionTransition* tptr){ return tptr->copy().release(); }
        );
    }

public:
    TransitionChain() {}

    TransitionChain(const TransitionChain& other) {
        copyFrom(other.transitionPtrs);
    }

    TransitionChain(const vector<PartitionTransition*>& transitionPtrs) {
        this->transitionPtrs = transitionPtrs;
    }

    TransitionChain inverse() {
        vector<PartitionTransition*> resultTransitionPtrs(transitionPtrs.size());

        transform(
                transitionPtrs.rbegin(),
                transitionPtrs.rend(),
                resultTransitionPtrs.begin(),
                [](PartitionTransition* tptr){ return tptr->inverse().release(); }
        );

        return TransitionChain(resultTransitionPtrs);
    }

    TransitionChain conjugate() {
        vector<PartitionTransition*> resultTransitionPtrs(transitionPtrs.size());

        transform(
                transitionPtrs.begin(),
                transitionPtrs.end(),
                resultTransitionPtrs.begin(),
                [](PartitionTransition* tptr){ return tptr->conjugate().release(); }
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

    const PartitionTransition& operator[](int index) {
        return *transitionPtrs[index];
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
                result << " => ";
            }

            result << (*it)->toString();
        }

        result << "] | " << transitionPtrs.size();

        return result.str();
    }

    ~TransitionChain() {
        for (auto it = transitionPtrs.begin(); it != transitionPtrs.end(); it++) {
            delete *it;
        }
    }
};

ostream &operator<<(ostream &strm, const Partition &partition);

ostream &operator<<(ostream &strm, const Color &color);

ostream &operator<<(ostream &strm, const ColoredPartition &partition);

ostream &operator<<(ostream &strm, const PartitionTransition &transition);

ostream &operator<<(ostream &strm, const TransitionChain &transitions);

#endif //LIMIT_GRAPH_PARTITION_HPP
