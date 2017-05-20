#ifndef LIMIT_GRAPH_PARTITION_HPP
#define LIMIT_GRAPH_PARTITION_HPP

#include "graph.hpp"

using namespace std;

class Partition {
protected:
    unsigned int num;
    vector<unsigned int> content;

public:
    Partition(const vector<unsigned int>& content);

    Partition(const Partition& other);

    static Partition from(int columns, int rows);

    static Partition from(const Graph& graph);

    void move(int from, int to);

    void insert(int columnIndex);

    void remove(int columnIndex);

    void replaceTail(Partition& newTail);

    bool isValid();

    bool isMaximumGraphical() const;

    bool isGraphical();

    unsigned int sum() const;

    unsigned int length() const;

    unsigned int rank() const;

    int rightmostByRow(int rowIndex) const;

    int rightmostByColumn(int columnIndex);

    Partition head() const;

    Partition tail() const;

    Partition conjugate();

    unique_ptr<vector<Partition>> graphicalChildrenPtr() const;

    bool operator==(const Partition& other) const;

    bool operator!=(const Partition& other) const;

    bool operator<=(const Partition& other) const;

    bool operator>=(const Partition& other) const;

    bool operator<(const Partition& other) const;

    bool operator>(const Partition& other) const;

    unsigned int operator[](int index) const;

    string toString() const;
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
    ColoredPartition(const vector<unsigned int> &content);

    ColoredPartition(const Partition &other);

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

#endif //LIMIT_GRAPH_PARTITION_HPP
