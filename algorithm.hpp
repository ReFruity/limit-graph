#ifndef LIMIT_GRAPH_ALGORITHM_HPP
#define LIMIT_GRAPH_ALGORITHM_HPP

#include "graph.hpp"
#include "partition.hpp"
#include "transition.hpp"
#include <iostream>
#include <unordered_set>
#include <unordered_map>

// region Algorithm

void greedyEdgeRotation(Graph &graph);

unique_ptr<Graph> randomGraphPtr(unsigned int size, unsigned int seed);

TransitionChain partitionTransitionChain(Partition from, Partition to);

TransitionChain headTailConjugateChain(Partition &partition);

TransitionChain inverseGraphicallyMaximizingChain(Partition& partition);

void partitionGraphicalAscendants(const Partition& partition, vector<Partition>& output);

void partitionBasicGraphicalAscendants(const Partition& partition, vector<Partition>& output);

unique_ptr<deque<Partition>> findShortestMaximizingChainPtr(const Partition& startPartition);

// endregion

#endif //LIMIT_GRAPH_ALGORITHM_HPP