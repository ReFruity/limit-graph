#ifndef THRESHOLD_GRAPH_TEST_HPP
#define THRESHOLD_GRAPH_TEST_HPP

#include <iostream>
#include <cassert>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include "graph.hpp"
#include "partition.hpp"
#include "transition.hpp"
#include "algorithm.hpp"

class LimitGraphTest
{
public:
    static void all();

    static void partition();

    static void graph();

    static void transition();

    static void algorithm();
};

ostream &operator<<(ostream &strm, vector<int> numbers);

#endif //THRESHOLD_GRAPH_TEST_HPP
