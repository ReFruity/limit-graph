#include <iostream>
#include <cassert>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include "graph.hpp"
#include "partition.hpp"
#include "transition.hpp"
#include "algorithm.hpp"
#include "test.hpp"

using namespace std;

int graphMain(int argc, char *argv[]) {
    unique_ptr<Graph> graphPtr;

    if (argc == 3) {
        unsigned int graphSize = (unsigned int) atoi(argv[1]);
        unsigned int seed = (unsigned int) atoi(argv[2]);

        cout << "Graph size: " << graphSize << endl;
        cout << "Random seed: " << seed << endl;

        graphPtr = randomGraphPtr(graphSize, seed);
    }
    else {
        graphPtr = unique_ptr<Graph>(new Graph(
            {{0, 1, 0, 0},
             {1, 0, 1, 0},
             {0, 1, 0, 1},
             {0, 0, 1, 0}}
        ));
        //graphPtr = randomGraphPtr(100, 0);
    }

    greedyEdgeRotation(*graphPtr);

    return 0;
}

int partitionMain(int argc, char *argv[]) {
    if (argc == 1) {
        cout << "Please specify partition as arguments. For example 3 3 2 1 1 1 1." << endl;
        return 1;
    }

    vector<unsigned int> inputs;

    for (int i = 1; i < argc; i++) {
        inputs.push_back(atoi(argv[i]));
    }

    Partition partition(inputs);

    if (!partition.isValid()) {
        cout << "You entered invalid partition. Terminating." << endl;
        return 1;
    }

    if (!partition.isGraphical()) {
        cout << "You entered not graphical partition. Terminating." << endl;
        return 1;
    }

    unique_ptr<deque<Partition>> chain(findShortestMaximizingChainPtr(partition));

    for(auto it = chain->begin(); it != chain->end(); it++) {
        cout << *it << endl;
    }

    return 0;
};

int experimentalMain(int argc, char *argv[]) {
    unique_ptr<Graph> graphPtr;

    if (argc == 3) {
        unsigned int graphSize = (unsigned int) atoi(argv[1]);
        unsigned int seed = (unsigned int) atoi(argv[2]);

        cout << "Graph size: " << graphSize << endl;
        cout << "Random seed: " << seed << endl << endl;

        graphPtr = randomGraphPtr(graphSize, seed);
    }
    else {
        graphPtr = unique_ptr<Graph>(new Graph({
                 {0, 1, 0, 0},
                 {1, 0, 1, 0},
                 {0, 1, 0, 1},
                 {0, 0, 1, 0},
        }));
        graphPtr = unique_ptr<Graph>(new Graph({
                {0, 0, 1, 1, 0},
                {0, 0, 1, 0, 0},
                {1, 1, 0, 1, 0},
                {1, 0, 1, 0, 0},
                {0, 0, 0, 0, 0},
        }));

        cout << "Usage: program graphSize seed" << "endl";
        //graphPtr = randomGraphPtr(100, 0);
    }

    Partition partition(Partition::from(*graphPtr));

    cout << "Graph: " << endl << *graphPtr << endl << endl;

    cout << "Partition: " << endl << partition << endl << endl;

    cout << "Maximizing chain: " << endl;
    cout << inverseGraphicallyMaximizingChain(partition).inverse() << endl << endl;
}

int main(int argc, char *argv[]) {
    LimitGraphTest::all();

    //return graphMain(argc, argv);
    return partitionMain(argc, argv);

}

// TODO: Rename project 'Threshold graph'
// TODO: Make API 0.1 -> Print out every maximum graphical partitions above specified partition, label MGPs closest to specified
