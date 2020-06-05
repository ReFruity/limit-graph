#include "transition.hpp"

// region PartitionTransition

bool PartitionTransition::operator!=(const PartitionTransition& other) {
    return !((*this) == other);
}

// endregion

// region PartitionInsert

PartitionInsert::PartitionInsert(int columnIndex, int rowIndex)
        : columnIndex(columnIndex), rowIndex(rowIndex)
{}

unique_ptr<PartitionTransition> PartitionInsert::copy() {
    return unique_ptr<PartitionTransition>(new PartitionInsert(columnIndex, rowIndex));
}

void PartitionInsert::apply(Partition& partition) {
    partition.insert(columnIndex);
}

void PartitionInsert::apply(ColoredPartition& cpartition) {
    cpartition.insert(columnIndex);
}

unique_ptr<PartitionTransition> PartitionInsert::inverse() {
    return unique_ptr<PartitionTransition>(new PartitionRemove(columnIndex, rowIndex));
}

unique_ptr<PartitionTransition> PartitionInsert::conjugate() {
    return unique_ptr<PartitionTransition>(new PartitionInsert(rowIndex, columnIndex));
}

bool PartitionInsert::isAscending() {
    return true;
}

bool PartitionInsert::isDescending() {
    return false;
}

bool PartitionInsert::isIdentical() {
    return false;
}

bool PartitionInsert::isMove() const {
    return false;
}

bool PartitionInsert::isInsert() const {
    return true;
}

bool PartitionInsert::isRemove() const {
    return false;
}

int PartitionInsert::insertColumn() const {
    return columnIndex;
}

int PartitionInsert::insertRow() const {
    return rowIndex;
}

int PartitionInsert::removeColumn() const {
    stringstream message;
    message << "Call of PartitionInsert::removeColumn for " << toString() << ".";
    throw runtime_error(message.str());
}

int PartitionInsert::removeRow() const {
    stringstream message;
    message << "Call of PartitionInsert::removeRow for " << toString() << ".";
    throw runtime_error(message.str());
}

bool PartitionInsert::operator==(const PartitionTransition& other) {
    auto otherPtr = dynamic_cast<const PartitionInsert*>(&other);

    if (otherPtr == nullptr) {
        return false;
    }

    return otherPtr->columnIndex == columnIndex && otherPtr->rowIndex == rowIndex;
}

string PartitionInsert::toString() const {
    stringstream result;
    result << "(" << "+" << columnIndex << "," << rowIndex << ")";
    return result.str();
}

// endregion

// region PartitionMove

string PartitionMove::toString() const {
    stringstream result;
    result << "(" << fromColumn << "," << fromRow << "->" << toColumn << "," << toRow << ")";
    return result.str();
}

PartitionMove::PartitionMove(int fromColumn, int fromRow, int toColumn, int toRow)
        : fromColumn(fromColumn), fromRow(fromRow), toColumn(toColumn), toRow(toRow)
{}

unique_ptr<PartitionTransition> PartitionMove::copy() {
    return unique_ptr<PartitionTransition>(new PartitionMove(fromColumn, fromRow, toColumn, toRow));
}

void PartitionMove::apply(Partition& partition) {
    partition.move(fromColumn, toColumn);
}

void PartitionMove::apply(ColoredPartition& cpartition) {
    cpartition.move(fromColumn, toColumn);
}

unique_ptr<PartitionTransition> PartitionMove::inverse() {
    return unique_ptr<PartitionTransition>(new PartitionMove(toColumn, toRow, fromColumn, fromRow));
}

unique_ptr<PartitionTransition> PartitionMove::conjugate() {
    return unique_ptr<PartitionTransition>(new PartitionMove(fromRow, fromColumn, toRow, toColumn));
}

bool PartitionMove::isAscending() {
    return fromColumn < toColumn;
}

bool PartitionMove::isDescending() {
    return fromColumn > toColumn;
}

bool PartitionMove::isIdentical() {
    return fromColumn == toColumn;
}

bool PartitionMove::isMove() const {
    return true;
}

bool PartitionMove::isInsert() const {
    return false;
}

bool PartitionMove::isRemove() const {
    return false;
}

int PartitionMove::insertColumn() const {
    return toColumn;
}

int PartitionMove::insertRow() const {
    return toRow;
}

int PartitionMove::removeColumn() const {
    return fromColumn;
}

int PartitionMove::removeRow() const {
    return fromRow;
}

bool PartitionMove::operator==(const PartitionTransition& other) {
    auto otherPtr = dynamic_cast<const PartitionMove*>(&other);

    if (otherPtr == nullptr) {
        return false;
    }

    return otherPtr->fromColumn == fromColumn
           && otherPtr->fromRow == fromRow
           && otherPtr->toColumn == toColumn
           && otherPtr->toRow == toRow
            ;
}

// endregion

// region PartitionRemove

PartitionRemove::PartitionRemove(int columnIndex, int rowIndex)
        : columnIndex(columnIndex), rowIndex(rowIndex)
{}

unique_ptr<PartitionTransition> PartitionRemove::copy() {
    return unique_ptr<PartitionTransition>(new PartitionRemove(columnIndex, rowIndex));
}

void PartitionRemove::apply(Partition& partition) {
    partition.remove(columnIndex);
}

void PartitionRemove::apply(ColoredPartition& cpartition) {
    cpartition.remove(columnIndex);
}

unique_ptr<PartitionTransition> PartitionRemove::inverse() {
    return unique_ptr<PartitionTransition>(new PartitionInsert(columnIndex, rowIndex));
}

unique_ptr<PartitionTransition> PartitionRemove::conjugate() {
    return unique_ptr<PartitionTransition>(new PartitionRemove(rowIndex, columnIndex));
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

bool PartitionRemove::isMove() const {
    return false;
}

bool PartitionRemove::isInsert() const {
    return false;
}

bool PartitionRemove::isRemove() const {
    return true;
}

int PartitionRemove::insertColumn() const {
    stringstream message;
    message << "Call of PartitionRemove::insertColumn for " << toString() << ".";
    throw runtime_error(message.str());
}

int PartitionRemove::insertRow() const {
    stringstream message;
    message << "Call of PartitionRemove::insertRow for " << toString() << ".";
    throw runtime_error(message.str());
}

int PartitionRemove::removeColumn() const {
    return columnIndex;
}

int PartitionRemove::removeRow() const {
    return rowIndex;
}

bool PartitionRemove::operator==(const PartitionTransition& other) {
    const PartitionRemove* otherPtr = dynamic_cast<const PartitionRemove*>(&other);

    if (otherPtr == nullptr) {
        return false;
    }

    return otherPtr->columnIndex == columnIndex && otherPtr->rowIndex == rowIndex;
}

string PartitionRemove::toString() const {
    stringstream result;
    result << "(" << "-" << columnIndex << "," << rowIndex << ")";
    return result.str();
}

// endregion

// region TransitionChain

void TransitionChain::copyFrom(const vector<PartitionTransition *>& transitionPtrs) {
    this->transitionPtrs = vector<PartitionTransition*>(transitionPtrs.size());

    transform(
            transitionPtrs.begin(),
            transitionPtrs.end(),
            this->transitionPtrs.begin(),
            [](PartitionTransition* tptr){ return tptr->copy().release(); }
    );
}

TransitionChain::TransitionChain() {}

TransitionChain::TransitionChain(const TransitionChain& other) {
    copyFrom(other.transitionPtrs);
}

TransitionChain::TransitionChain(const vector<PartitionTransition *>& transitionPtrs) {
    this->transitionPtrs = transitionPtrs;
}

TransitionChain TransitionChain::inverse() {
    vector<PartitionTransition*> resultTransitionPtrs(transitionPtrs.size());

    transform(
            transitionPtrs.rbegin(),
            transitionPtrs.rend(),
            resultTransitionPtrs.begin(),
            [](PartitionTransition* tptr){ return tptr->inverse().release(); }
    );

    return TransitionChain(resultTransitionPtrs);
}

TransitionChain TransitionChain::conjugate() {
    vector<PartitionTransition*> resultTransitionPtrs(transitionPtrs.size());

    transform(
            transitionPtrs.begin(),
            transitionPtrs.end(),
            resultTransitionPtrs.begin(),
            [](PartitionTransition* tptr){ return tptr->conjugate().release(); }
    );

    return TransitionChain(resultTransitionPtrs);
}

void TransitionChain::push_back(PartitionTransition *transitionPtr) {
    transitionPtrs.push_back(transitionPtr);
}

void TransitionChain::apply(Partition& partition) {
    for (auto it = transitionPtrs.begin(); it != transitionPtrs.end(); ++it) {
        (*it)->apply(partition);
    }
}

int TransitionChain::length() {
    return transitionPtrs.size();
}

const PartitionTransition& TransitionChain::operator[](int index) {
    return *transitionPtrs[index];
}

TransitionChain& TransitionChain::operator=(const TransitionChain& other) {
    copyFrom(other.transitionPtrs);
    return *this;
}

bool TransitionChain::operator==(const TransitionChain& other) {
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

string TransitionChain::toString() const {
    stringstream result;
    result << "[";

    if (!transitionPtrs.empty()) {
        auto it = transitionPtrs.begin();
        for (; it != prev(transitionPtrs.end()); ++it) {
            result << (*it)->toString();
            result << " => ";
        }

        result << (*it)->toString();
    }

    result << "] | " << transitionPtrs.size();

    return result.str();
}

TransitionChain::~TransitionChain() {
    for (auto it = transitionPtrs.begin(); it != transitionPtrs.end(); ++it) {
        delete *it;
    }
}

// endregion

// region Output

ostream &operator<<(ostream &strm, const PartitionTransition &transition) {
    return strm << transition.toString();
}

ostream &operator<<(ostream &strm, const TransitionChain &transitions) {
    return strm << transitions.toString();
}

// endregion
