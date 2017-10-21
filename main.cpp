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

void graphMain(int argc, char *argv[]) {
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
}

void partitionMain(int argc, char *argv[]) {
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

        //graphPtr = randomGraphPtr(100, 0);
    }

    Partition partition(Partition::from(*graphPtr));

    cout << "Graph: " << endl << *graphPtr << endl << endl;

    cout << "Partition: " << endl << partition << endl << endl;

    cout << "Maximizing chain: " << endl;
    cout << inverseGraphicallyMaximizingChain(partition).inverse() << endl << endl;
};

int main(int argc, char *argv[]) {
    LimitGraphTest::test();

    //graphMain(argc, argv);
    partitionMain(argc, argv);

    return 0;
}

// TODO: Rename project 'Threshold graph'
