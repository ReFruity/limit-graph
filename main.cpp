#include <iostream>
#include <deque>
#include <unordered_set>
#include <unordered_map>
#include "partition.hpp"
#include "transition.hpp"
#include "algorithm.hpp"
#include "test.hpp"

using namespace std;

int graphMain(int argc, char *argv[]) {
    unique_ptr<Graph> graphPtr;

    if (argc == 3) {
        auto graphSize = (unsigned int) atoi(argv[1]);
        auto seed = (unsigned int) atoi(argv[2]);

        cout << "Graph size: " << graphSize << endl;
        cout << "Random seed: " << seed << endl;

        graphPtr = randomGraphPtr(graphSize);
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
        cout << "Finds every maximum graphical partition above the specified partition." << endl;
        cout << "Please specify the partition as descending integers. " << endl;
        cout << "For example 3 3 2 1 1 1 1." << endl;
        return 0;
    }

    vector<unsigned int> inputs;

    for (int i = 1; i < argc; i++) {
        inputs.push_back(atoi(argv[i]));
    }

    Partition partition(inputs);

    if (!partition.isValid()) {
        cout << "You entered invalid partition. Terminating." << endl;
        return 0;
    }

    if (!partition.isGraphical()) {
        cout << "You entered not graphical partition. Terminating." << endl;
        return 0;
    }

    PartitionSearchAlgorithm algo(partition);
    vector<Partition> partitions = *algo.getPartitions();
    vector<int> distances = *algo.getDistances();

    for (int i = 0; i < partitions.size(); i++) {
        cout << partitions[i] << " [Rank: " << partition.rank() << "] [Distance: " << distances[i] << "]" << endl;
    }

    return 0;
}

int experimentalMain(int argc, char *argv[]) {
    unique_ptr<Graph> graphPtr;

    if (argc == 3) {
        unsigned int graphSize = (unsigned int) atoi(argv[1]);
        unsigned int seed = (unsigned int) atoi(argv[2]);

        cout << "Graph size: " << graphSize << endl;
        cout << "Random seed: " << seed << endl << endl;

        graphPtr = randomGraphPtr(graphSize);
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

int partitionStatMain(int argc, char *argv[]) {
    if (argc != 4) {
        cout << "Generates random graphical partition for graph with specified size." << endl;
        cout << "Then finds every maximum graphical partition above them." << endl;
        cout << "Please specify graph size, random seed and iterations as integer arguments." << endl;
        return 0;
    }

    unsigned int graphSize = (unsigned int) atoi(argv[1]);
    unsigned int randomSeed = (unsigned int) atoi(argv[2]);
    unsigned int iterations = (unsigned int) atoi(argv[3]);

    srand(randomSeed);

    for (int i = 0; i < iterations; i++) {
        cout << "Iteration " << i << endl;
        Partition partition = *randomGraphPartitionPtr(graphSize);
        cout << "Partition " << partition << endl;
        PartitionSearchAlgorithm algo(partition);
        vector<Partition> partitions = *algo.getPartitions();
        vector<int> distances = *algo.getDistances();

        cout << "Rank, Distance, Partition" << endl;

        for (int j = 0; j < partitions.size(); j++) {
            cout << partitions[j].rank() << "," << distances[j] << "," << partitions[j].toCSV() << endl;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    LimitGraphTest::all();

    //return graphMain(argc, argv);
    //return partitionMain(argc, argv);
    return partitionStatMain(argc, argv);
}

// TODO: Parse arguments like 7x1