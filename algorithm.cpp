#include "algorithm.hpp"

void greedyEdgeRotation(Graph& graph) {
    int rotations = 0;
    unique_ptr<Triple> triplePtr;
    cout << graph << endl;

    while((triplePtr = graph.maxIncreasingTriplePtr()) != nullptr) {
        graph.rotateEdge(triplePtr);
        rotations++;
        cout << "Rotation #" << rotations << " " << *triplePtr << endl;
        cout << graph << endl;
    }

    cout << "Total: " << rotations << " rotations." << endl;
}

unique_ptr<Graph> randomGraphPtr(unsigned int size, unsigned int seed) {
    vector<vector<short>> adjacencyMatrix(size, vector<short>(size, 0));

    srand(seed);
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < i; j++) {
            short randomNum = (short) (rand() % 2);
            adjacencyMatrix[i][j] = adjacencyMatrix[j][i] = randomNum;
        }
    }

    return unique_ptr<Graph>(new Graph(adjacencyMatrix));
}

TransitionChain partitionTransitionChain(Partition from, Partition to) {
    if (!(from <= to)) {
        throw invalid_argument("Arguments must satisfy: from <= to");
    }

    TransitionChain result;
    int maxLength = max(from.length(), to.length());
    int searchIndex = 0;

    for (int i = 0; i < maxLength; i++) {
        if (from[i] <= to[i]) {
            continue;
        }

        for (int j = searchIndex; j < maxLength; j++) {
            if (from[j] >= to[j]) {
                continue;
            }

            int fromRightmost = from.rightmostByColumn(i);
            result.push_back(new PartitionMove(fromRightmost, from[fromRightmost] - 1, j, from[j]));
            from.move(fromRightmost, j);

            if (from[j] == to[j]) {
                searchIndex = j + 1;
            }
            i--;

            break;
        }
    }

    for (int i = searchIndex; i < maxLength; i++) {
        if (from[i] >= to[i]) {
            continue;
        }

        for (int j = to[i] - from[i]; j > 0; j--) {
            result.push_back(new PartitionInsert(i, to[i] - j));
        }
    }

    return result;
}

TransitionChain headTailConjugateChain(Partition& partition) {
    return partitionTransitionChain(partition.head(), partition.tail()).conjugate();
}

TransitionChain inverseGraphicallyMaximizingChain(Partition& partition) {
    TransitionChain mainChain = headTailConjugateChain(partition);

    ColoredPartition maximumPartition(partition);
    maximumPartition.maximize();
    int rank = maximumPartition.rank();

    TransitionChain result;

    for (int i = 0; i < mainChain.length(); i++) {
        if (mainChain[i].isInsert()) {
            int toColumn = mainChain[i].insertColumn() + rank;
            int toRow = mainChain[i].insertRow();

            if (maximumPartition.hasBlock(toColumn, toRow)) {
                // It is proven that this block is GREY
                maximumPartition.paint(BLACK, toColumn, toRow);
            }
            else {
                bool greyBlockMoved = false;

                for (int fromColumn = toColumn - 1; fromColumn > rank; fromColumn--) {
                    for (int fromRow = rank - 1; fromRow >= 0; fromRow--) {
                        if (maximumPartition.hasBlock(fromColumn, fromRow)
                            && maximumPartition.getColor(fromColumn, fromRow) == GREY) {
                            PartitionTransition* move = new PartitionMove(fromColumn, fromRow, toColumn, toRow);
                            move->apply(maximumPartition);
                            maximumPartition.paint(BLACK, toColumn, toRow);
                            result.push_back(move);
                            greyBlockMoved = true;
                            break;
                        }
                    }

                    if (greyBlockMoved) {
                        break;
                    }
                }

                if (greyBlockMoved) {
                    continue;
                }

                for (int fromRow = maximumPartition[0] - 1; fromRow >= rank; fromRow--) {
                    for (int fromColumn = 0; fromColumn < rank; fromColumn++) {
                        if (maximumPartition.hasBlock(fromColumn, fromRow)
                            && maximumPartition.getColor(fromColumn, fromRow) == GREY) {
                            PartitionTransition* move = new PartitionMove(fromColumn, fromRow, toColumn, toRow);
                            move->apply(maximumPartition);
                            maximumPartition.paint(BLACK, toColumn, toRow);
                            result.push_back(move);
                            greyBlockMoved = true;
                            break;
                        }
                    }

                    if (greyBlockMoved) {
                        break;
                    }
                }
            }

            continue;
        }

        if (mainChain[i].isMove()) {
            int fromColumn = mainChain[i].removeColumn() + rank;
            int fromRow = mainChain[i].removeRow();

            int toColumn = mainChain[i].insertColumn() + rank;
            int toRow = mainChain[i].insertRow();

            if (maximumPartition.hasBlock(toColumn, toRow)) {
                // It is proven that this block is GREY
                maximumPartition.paint(BLACK, toColumn, toRow);
                maximumPartition.paint(GREY, fromColumn, fromRow);
            }
            else {
                bool greyBlockMoved = false;

                for (int column = toColumn - 1; column >= fromColumn; column--) {
                    for (int row = rank - 1; row >= 0; row--) {
                        if (maximumPartition.hasBlock(column, row)
                            && maximumPartition.getColor(column, row) == GREY) {
                            PartitionTransition* move = new PartitionMove(column, row, toColumn, toRow);
                            move->apply(maximumPartition);
                            maximumPartition.paint(BLACK, toColumn, toRow);
                            result.push_back(move);
                            greyBlockMoved = true;
                            break;
                        }
                    }

                    if (greyBlockMoved) {
                        break;
                    }
                }

                if (greyBlockMoved) {
                    continue;
                }

                PartitionTransition* move = new PartitionMove(fromColumn, fromRow, toColumn, toRow);
                move->apply(maximumPartition);
                result.push_back(move);
            }

            continue;
        }
    }

    return result;
}

void partitionGraphicalAscendants(const Partition& partition, vector<Partition>& output) {
    unsigned int length = partition.length();

    for (int i = 0; i < length - 1; i++) {
        if (!partition.isInsertable(i)) {
            continue;
        }

        for (int j = i + 1; j < length; j++) {
            if (partition.isRemovable(j)) {
                Partition child(partition);
                child.move(j, i);

                if (child.isGraphical()) {
                    output.push_back(child);
                }
            }
        }
    }
}

void partitionBasicGraphicalAscendants(const Partition& partition, vector<Partition>& output) {
    for (int i = partition.length() - 1; i >= 0; i--) {
        if (partition[i + 1] == partition[i]) {
            continue;
        }

        for (int j = i - 1; j >= 0; j--) {
            if (j == 0 || partition[j - 1] > partition[i - 1]) {
                Partition child(partition);
                child.move(i, j);

                if (child.isGraphical()) {
                    output.push_back(child);
                }

                break;
            }
        }
    }
}

unique_ptr<vector<Partition>> findShortestMaximizingChainPtr2(const Partition& startPartition) {
    unique_ptr<vector<Partition>> storagePtr(new vector<Partition>());
    deque<const Partition*> queue({&startPartition});
    unordered_set<const Partition*> visited({&startPartition});
    unordered_map<const Partition*, const Partition*> parent;

    int stack = 0;
    int* heap = new int(0);

    cout << "Stack: " << &stack << endl;
    cout << "Heap: " << heap << endl << endl;

    while (!queue.empty()) {
        cout << "Queue front: " << endl;
        cout << *queue.front() << " &" << queue.front() << endl;

        const Partition& partition = *queue.front();

        cout << "Partition: " << endl << partition << " &" << &partition << endl;

        queue.pop_front();

        cout << "Queue: " << endl;
        for_each(queue.begin(), queue.end(), [](const Partition* p){ cout << *p << " &" << p << endl; });

        //if (partition.isMaximumGraphical()) {
        //    cout << "Start partition: " << endl;
        //    cout << startPartition << endl;
        //    cout << "End partition: " << endl;
        //    cout << partition << endl;
        //
        //    const Partition* partitionPtr(&partition);
        //    unique_ptr<vector<Partition>> result(new vector<Partition>());
        //
        //    while (partitionPtr != &startPartition) {
        //        partitionPtr = parent[partitionPtr];
        //        result->push_back(Partition(*partitionPtr));
        //    }
        //
        //    return result;
        //}

        int ascendantsIndex = storagePtr->size();

        partitionBasicGraphicalAscendants(partition, *storagePtr);

        //unique_ptr<vector<Partition>> childPartitionsPtr(partition.graphicalChildrenPtr());

        // TODO: THIS LINE CAUSES MY HEADACHE! (partition becomes invalid reference)
        //storagePtr->insert(back_inserter(storagePtr), childPartitionsPtr->begin(), childPartitionsPtr->end());
        //copy(childPartitionsPtr->begin(), childPartitionsPtr->end(), back_inserter(*storagePtr));

        //for (auto it = childPartitionsPtr->begin(); it != childPartitionsPtr->end(); ++it) {
        //    cout << "*it: " << endl << *it << endl;
        //    storagePtr->push_back(*it);
        //}

        cout << "Partition after copy: " << endl << partition << " &" << &partition << endl;

        //assert(storagePtr->size() >= childPartitionsPtr->size());

        //for (auto it = storagePtr->end() - childPartitionsPtr->size(); it != storagePtr->end(); ++it) {
        //    queue.push_back(&*it);
        //    parent[&*it] = &partition;
        //}

        //for (int i = storagePtr->size() - childPartitionsPtr->size(); i < storagePtr->size(); i++) {
        for (int i = ascendantsIndex; i < storagePtr->size(); i++) {
            queue.push_back(&(*storagePtr)[i]);
            parent[&(*storagePtr)[i]] = &partition;
        }

        //visited.insert(&partition);

        cout << "Partition: " << endl << partition << " &" << &partition << endl;

        //cout << "Child partitions: " << endl;
        //for_each(childPartitionsPtr->begin(), childPartitionsPtr->end(), [](const Partition& p){ cout << p << " &" << &p << endl; });

        cout << "Storage: " << endl;
        for_each(storagePtr->begin(), storagePtr->end(), [](const Partition& p){ cout << p << " &" << &p << endl; });

        cout << "Queue: " << endl;
        for_each(queue.begin(), queue.end(), [](const Partition* p){ cout << *p << " &" << p << endl; });

        cout << "Visited: " << endl;
        for_each(visited.begin(), visited.end(), [](const Partition* p){ cout << *p << " &" << p << endl; });

        cout << "Parent: " << endl;
        for_each(parent.begin(), parent.end(), [](const pair<const Partition*, const Partition*> pair){
            cout << *pair.first << " &" << pair.first << " -> " << *pair.second << " &" << pair.second << endl;
        });
        cout << endl;
    }

    stringstream message;
    message << "Invalid state: didn't find maximum graphical partition from '" << startPartition << "'.";
    throw runtime_error(message.str());
}

unique_ptr<deque<Partition>> findShortestMaximizingChainPtr(const Partition& startPartition) {
    deque<Partition> queue({startPartition});
    unordered_set<Partition> visited;
    unordered_map<Partition, Partition> parent;

    while (!queue.empty()) {
        Partition partition = queue.front();
        queue.pop_front();

        if (partition.isMaximumGraphical()) {
            unique_ptr<deque<Partition>> result(new deque<Partition>({partition}));

            while (partition != startPartition) {
                result->push_front((Partition&&) parent.at(partition));
                partition = parent.at(partition);
            }

            return result;
        }

        vector<Partition> graphicalAscendants;
        partitionGraphicalAscendants(partition, graphicalAscendants);

        for (int i = 0; i < graphicalAscendants.size(); i++) {
            const Partition& child = graphicalAscendants[i];

            if (visited.count(child) > 0) {
                continue;
            }

            queue.push_back(child);
            parent.insert({child, partition});
        }

        visited.insert(partition);
    }

    stringstream message;
    message << "Invalid state: didn't find maximum graphical partition from '" << startPartition << "'.";
    throw runtime_error(message.str());
}
