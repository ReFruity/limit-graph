#ifndef LIMIT_GRAPH_ALGORITHM_HPP
#define LIMIT_GRAPH_ALGORITHM_HPP

#include "graph.hpp"
#include "partition.hpp"
#include "transition.hpp"
#include <iostream>
#include <unordered_set>
#include <unordered_map>

void greedyEdgeRotation(Graph &graph);

unique_ptr<Graph> randomGraphPtr(unsigned int size, unsigned int seed);

unique_ptr<Partition> randomPartitionPtr(unsigned int graphSize, unsigned int seed);

TransitionChain partitionTransitionChain(Partition from, Partition to);

TransitionChain headTailConjugateChain(Partition &partition);

TransitionChain inverseGraphicallyMaximizingChain(Partition& partition);

void partitionGraphicalAscendants(const Partition& partition, vector<Partition>& output);

void partitionBasicGraphicalAscendants(const Partition& partition, vector<Partition>& output);

unique_ptr<deque<Partition>> findShortestMaximizingChainPtr(const Partition& startPartition);

unique_ptr<unordered_set<Partition>> findMaximumGraphicalPartitionsPtr(const Partition& startPartition);

class PartitionSearchAlgorithm {
private:
    Partition partition;
    vector<Partition> partitions;
    vector<int> distances;
public:
    PartitionSearchAlgorithm(Partition graphicalPartition);

    unique_ptr<vector<Partition>> getPartitions();

    unique_ptr<vector<int>> getDistances();
};

#endif //LIMIT_GRAPH_ALGORITHM_HPP

//TODO: Refactor algos to return unique_ptr as a result
