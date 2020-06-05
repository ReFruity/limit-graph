#ifndef THRESHOLD_GRAPH_TRANSITION_HPP
#define THRESHOLD_GRAPH_TRANSITION_HPP

#include "partition.hpp"

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

    bool operator!=(const PartitionTransition& other);

    virtual string toString() const = 0;
};

class PartitionInsert : public PartitionTransition {
private:
    int columnIndex, rowIndex;

public:
    PartitionInsert(int columnIndex, int rowIndex);

    unique_ptr<PartitionTransition> copy();

    void apply(Partition& partition);

    void apply(ColoredPartition& cpartition);

    unique_ptr<PartitionTransition> inverse();

    unique_ptr<PartitionTransition> conjugate();

    bool isAscending();

    bool isDescending();

    bool isIdentical();

    bool isMove() const;

    bool isInsert() const;

    bool isRemove() const;

    int insertColumn() const;

    int insertRow() const;

    int removeColumn() const;

    int removeRow() const;

    bool operator==(const PartitionTransition& other);

    string toString() const;
};

class PartitionMove : public PartitionTransition {
private:
    int fromColumn, fromRow, toColumn, toRow;

public:
    PartitionMove(int fromColumn, int fromRow, int toColumn, int toRow);

    unique_ptr<PartitionTransition> copy();

    void apply(Partition& partition);

    void apply(ColoredPartition& cpartition);

    unique_ptr<PartitionTransition> inverse();

    unique_ptr<PartitionTransition> conjugate();

    bool isAscending();

    bool isDescending();

    bool isIdentical();

    bool isMove() const;

    bool isInsert() const;

    bool isRemove() const;

    int insertColumn() const;

    int insertRow() const;

    int removeColumn() const;

    int removeRow() const;

    bool operator==(const PartitionTransition& other);

    string toString() const;
};

class PartitionRemove : public PartitionTransition {
private:
    int columnIndex, rowIndex;

public:
    PartitionRemove(int columnIndex, int rowIndex);

    unique_ptr<PartitionTransition> copy();

    void apply(Partition& partition);

    void apply(ColoredPartition& cpartition);

    unique_ptr<PartitionTransition> inverse();

    unique_ptr<PartitionTransition> conjugate();

    bool isAscending();

    bool isDescending();

    bool isIdentical();

    bool isMove() const;

    bool isInsert() const;

    bool isRemove() const;

    bool operator==(const PartitionTransition& other);

    string toString() const;

    int insertColumn() const;

    int insertRow() const;

    int removeColumn() const;

    int removeRow() const;
};

class TransitionChain {
private:
    vector<PartitionTransition*> transitionPtrs;

    void copyFrom(const vector<PartitionTransition*>& transitionPtrs);

public:
    TransitionChain();

    TransitionChain(const TransitionChain& other);

    TransitionChain(const vector<PartitionTransition*>& transitionPtrs);

    TransitionChain inverse();

    TransitionChain conjugate();

    void push_back(PartitionTransition* transitionPtr);

    void apply(Partition& partition);

    int length();

    const PartitionTransition& operator[](int index);

    TransitionChain& operator=(const TransitionChain& other);

    bool operator==(const TransitionChain& other);

    string toString() const;

    ~TransitionChain();
};

ostream &operator<<(ostream &strm, const PartitionTransition &transition);

ostream &operator<<(ostream &strm, const TransitionChain &transitions);

#endif //THRESHOLD_GRAPH_TRANSITION_HPP
