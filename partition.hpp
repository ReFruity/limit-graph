#ifndef THRESHOLD_GRAPH_PARTITION_HPP
#define THRESHOLD_GRAPH_PARTITION_HPP

#include <deque>
#include "graph.hpp"

using namespace std;

class Partition {
private:
    unsigned int num;
    vector<unsigned int> content;

public:
    explicit Partition(const vector<unsigned int>& content);

    Partition(const Partition& other);

    static Partition from(int columns, int rows);

    static Partition from(const Graph& graph);

    void move(int from, int to);

    void insert(int columnIndex);

    void remove(int columnIndex);

    void replaceTail(Partition& newTail);

    bool isInsertable(int columnIndex) const;

    bool isRemovable(int columnIndex) const;

    bool isValid() const;

    bool isMaximumGraphical() const;

    bool isGraphical() const;

    unsigned int sum() const;

    unsigned int length() const;

    unsigned int rank() const;

    int rightmostByRow(int rowIndex) const;

    int rightmostByColumn(int columnIndex) const;

    Partition head() const;

    Partition tail() const;

    Partition conjugate();

    bool operator==(const Partition& other) const;

    bool operator!=(const Partition& other) const;

    bool operator<=(const Partition& other) const;

    bool operator>=(const Partition& other) const;

    bool operator<(const Partition& other) const;

    bool operator>(const Partition& other) const;

    unsigned int operator[](int index) const;

    size_t hashCode() const;

    string toString() const;

    // TODO: Remove
    ~Partition(){ num = 999999; }

    string toCSV() const;
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

    void resizeColorsVertically();

    void resizeColors();

public:
    explicit ColoredPartition(const vector<unsigned int> &content);

    explicit ColoredPartition(const Partition &other);

    ColoredPartition(const ColoredPartition &other);

    void move(int from, int to);

    void insert(int columnIndex);

    void remove(int columnIndex);

    void paint(Color color, int columnIndex);

    void paint(Color color, int columnIndex, int rowIndex);

    Color getColor(int columnIndex) const;

    Color getColor(int columnIndex, int rowIndex) const;

    bool hasBlock(int columnIndex, int rowIndex);

    void paintHeadBlack();

    void fillHead();

    void maximize();

    bool isValid();

    unsigned int sum();

    unsigned int length();

    unsigned int rank();

    bool operator==(const ColoredPartition& other) const;

    unsigned int operator[](int index) const;

    string toShortString() const;

    string toString() const;
};

ostream &operator<<(ostream &strm, const Partition &partition);

ostream &operator<<(ostream &strm, const Color &color);

ostream &operator<<(ostream &strm, const ColoredPartition &partition);

ostream &operator<<(ostream &strm, const vector<Partition> &partitionChain);

ostream &operator<<(ostream &strm, const deque<Partition> &partitionChain);

namespace std {
    template <>
    struct hash<Partition> {
        size_t operator()(const Partition& partition) const noexcept;
    };
}

#endif //THRESHOLD_GRAPH_PARTITION_HPP
