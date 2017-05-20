#include "partition.hpp"

// region PartitionRemove implementation

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

// region Output

ostream &operator<<(ostream &strm, const Partition &partition) {
    return strm << partition.toString();
}

ostream &operator<<(ostream &strm, const Color &color) {
    return strm << char(color);
}

ostream &operator<<(ostream &strm, const ColoredPartition &partition) {
    return strm << partition.toString();
}

ostream &operator<<(ostream &strm, const PartitionTransition &transition) {
    return strm << transition.toString();
}

ostream &operator<<(ostream &strm, const TransitionChain &transitions) {
    return strm << transitions.toString();
}

// endregion
