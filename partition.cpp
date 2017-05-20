#include "partition.hpp"

// region Partition

Partition::Partition(const vector<unsigned int>& content) :
        num(accumulate(content.begin(), content.end(), 0u)),
        content(content)
{}

Partition::Partition(const Partition& other) {
    num = other.num;
    content = std::vector<unsigned int>(other.content);
}

Partition Partition::from(int columns, int rows) {
    return Partition(vector<unsigned int>(columns, rows));
}

Partition Partition::from(const Graph& graph) {
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

void Partition::move(int from, int to) {
    if (to >= content.size()) {
        content.resize((unsigned int) to + 1);
    }

    content[from]--;
    content[to]++;
}

void Partition::insert(int columnIndex) {
    if (columnIndex >= content.size()) {
        content.resize((unsigned int) columnIndex + 1);
    }

    content[columnIndex]++;
    num++;
}

void Partition::remove(int columnIndex) {
    content[columnIndex]--;
    num--;
}

void Partition::replaceTail(Partition& newTail) {
    unsigned int thisRank = rank();
    unsigned int thisLength = length();

    for (int i = thisRank; i < thisLength; i++) {
        num = num - content[i] + newTail[i - thisRank];
        content[i] = newTail[i - thisRank];
    }
}

bool Partition::isValid() {
    if (num != accumulate(content.begin(), content.end(), 0u)) {
        return false;
    }

    if (!is_sorted(content.begin(), content.end(), greater<unsigned int>())) {
        return false;
    }

    return true;
}

bool Partition::isMaximumGraphical() const {
    return head() == tail();
}

bool Partition::isGraphical() const {
    return sum() % 2 == 0 && tail() >= head();
}

unsigned int Partition::sum() const {
    return num;
}

unsigned int Partition::length() const {
    for (unsigned int i = 0; i < content.size(); i++) {
        if (content[i] == 0) {
            return i;
        }
    }

    return content.size();
}

unsigned int Partition::rank() const {
    for (unsigned int i = 0; i < content.size(); i++) {
        if (content[i] <= i) {
            return i;
        }
    }

    return content.size();
}

int Partition::rightmostByRow(int rowIndex) const {
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

int Partition::rightmostByColumn(int columnIndex) {
    if ((*this)[columnIndex] == 0) {
        return columnIndex;
    }

    return rightmostByRow((*this)[columnIndex] - 1);
}

Partition Partition::head() const {
    int thisRank = rank();

    auto resultContent = vector<unsigned int>(content.begin(), content.begin() + thisRank);
    for_each(resultContent.begin(), resultContent.end(), [thisRank](unsigned int& val){ val -= (thisRank - 1); });

    return Partition(resultContent);
}

Partition Partition::tail() const {
    return Partition(vector<unsigned int>(content.begin() + rank(), content.end())).conjugate();
}

Partition Partition::conjugate() {
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

unique_ptr<vector<Partition>> Partition::graphicalChildrenPtr() const {
    unsigned int thisLength = length();
    unique_ptr<vector<Partition>> result(new vector<Partition>());

    for (int i = thisLength - 1; i >= 0; i--) {
        if ((*this)[i + 1] == (*this)[i]) {
            continue;
        }

        for (int j = i - 1; j >= 0; j--) {
            if (j == 0 || content[j - 1] > content[i - 1]) {
                Partition child(*this);
                child.move(i, j);

                if (child.isGraphical()) {
                    result->push_back(child);
                }

                break;
            }
        }
    }

    return result;
}

bool Partition::operator==(const Partition& other) const {
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

bool Partition::operator!=(const Partition& other) const {
    return !(*this == other);
}

bool Partition::operator<=(const Partition& other) const {
    return other >= *this;
}

bool Partition::operator>=(const Partition& other) const {
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

bool Partition::operator<(const Partition& other) const {
    return (*this) <= other && (*this) != other;
}

bool Partition::operator>(const Partition& other) const {
    return (*this) >= other && (*this) != other;
}

unsigned int Partition::operator[](int index) const {
    return index < content.size() ? content[index] : 0;
}

string Partition::toString() const {
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

// endregion

// region ColoredPartition

void ColoredPartition::resizeColorsVertically() {
    for (int i = 0; i < colors.size(); i++) {
        colors[i].resize(partition[0], NONE);
    }
}

void ColoredPartition::resizeColors() {
    unsigned int partitionLength = partition.length();

    if (partitionLength > colors.size()) {
        colors.resize(partitionLength);

        resizeColorsVertically();
    }

    if (partition[0] > colors[0].size()) {
        resizeColorsVertically();
    }
}

ColoredPartition::ColoredPartition(const vector<unsigned int>& content)
        : partition(content), colors(content.size(), vector<Color>(content[0], NONE))
{}

ColoredPartition::ColoredPartition(const Partition& other)
        : partition(other), colors(other.length(), vector<Color>(other[0], NONE))
{}

ColoredPartition::ColoredPartition(const ColoredPartition& other)
        : partition(other.partition), colors(other.colors)
{}

void ColoredPartition::move(int from, int to) {
    partition.move(from, to);
    resizeColors();
    colors[to][partition[to] - 1] = colors[from][partition[from]];
    colors[from][partition[from]] = NONE;
}

void ColoredPartition::insert(int columnIndex) {
    partition.insert(columnIndex);
    resizeColors();
}

void ColoredPartition::remove(int columnIndex) {
    partition.remove(columnIndex);
    colors[columnIndex][partition[columnIndex]] = NONE;
}

void ColoredPartition::paint(Color color, int columnIndex) {
    colors[columnIndex][partition[columnIndex] - 1] = color;
}

void ColoredPartition::paint(Color color, int columnIndex, int rowIndex) {
    colors[columnIndex][rowIndex] = color;
}

Color ColoredPartition::getColor(int columnIndex) const {
    if (columnIndex > colors.size()) {
        return NONE;
    }

    return colors[columnIndex][partition[columnIndex] - 1];
}

Color ColoredPartition::getColor(int columnIndex, int rowIndex) const {
    if (columnIndex > colors.size()) {
        return NONE;
    }

    if (rowIndex > partition[columnIndex] - 1) {
        return NONE;
    }

    return colors[columnIndex][rowIndex];
}

bool ColoredPartition::hasBlock(int columnIndex, int rowIndex) {
    return partition[columnIndex] > rowIndex;
}

void ColoredPartition::paintHeadBlack() {
    int thisRank = rank();

    for (int rowIndex = thisRank - 1; rowIndex < partition[0]; rowIndex++) {
        for (int columnIndex = 0; columnIndex < thisRank; columnIndex++) {
            if (hasBlock(columnIndex, rowIndex)) {
                paint(BLACK, columnIndex, rowIndex);
            }
        }
    }
}

void ColoredPartition::fillHead() {
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

void ColoredPartition::maximize() {
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

bool ColoredPartition::isValid() {
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

unsigned int ColoredPartition::sum() {
    return partition.sum();
}

unsigned int ColoredPartition::length() {
    return partition.length();
}

unsigned int ColoredPartition::rank() {
    return partition.rank();
}

bool ColoredPartition::operator==(const ColoredPartition& other) const {
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

unsigned int ColoredPartition::operator[](int index) const {
    return partition[index];
}

string ColoredPartition::toShortString() const {
    stringstream stringStream;

    stringStream << partition.toString() << endl;

    for (int i = 0; i < partition.length(); i++) {
        stringStream << char(getColor(i)) << " ";
    }

    return stringStream.str();
}

string ColoredPartition::toString() const {
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

// endregion
