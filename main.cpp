#include <cassert>
#include <set>
#include <queue>
#include <iostream>
#include "graph.hpp"
#include "partition.hpp"
#include "transition.hpp"

using namespace std;

// region Algorithm

void greedyEdgeRotation(Graph &graph) {
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
            assert(from.isValid());
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

TransitionChain headTailConjugateChain(Partition &partition) {
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

unique_ptr<TransitionChain> FindShortestMaximizingChainPtr(const Partition& startPartition) {
    unique_ptr<vector<Partition>> childPartitionsPtr = startPartition.graphicalChildrenPtr();
    set<Partition> visited({startPartition});
    deque<Partition> queue(childPartitionsPtr->begin(), childPartitionsPtr->end());
    //unordered_map<Partition, Partition> parent;

    while (!queue.empty()) {
        const Partition& partition = queue.front();
        queue.pop_front();

        if (partition.isMaximumGraphical()) {
            cout << "Start partition: " << endl;
            cout << startPartition << endl;
            cout << "End partition: " << endl;
            cout << partition << endl;
            return unique_ptr<TransitionChain>(new TransitionChain());
        }

        childPartitionsPtr = partition.graphicalChildrenPtr();
        queue.insert(queue.end(), childPartitionsPtr->begin(), childPartitionsPtr->end());
        //for (auto it = childPartitionsPtr->begin(); it != childPartitionsPtr->end(); it++) {
        //    parent[*it] = partition;
        //}
    }
}

// endregion

void test() {
    // region Graph

    vector<vector<short>> adjacencyMatrix;

    adjacencyMatrix = vector<vector<short>>({{0, 1}, {1, 0}});
    Graph graph = Graph(adjacencyMatrix);
    assert(graph.isLimit());
    assert(graph == Graph(adjacencyMatrix));

    adjacencyMatrix = vector<vector<short>>({{0, 1, 0}, {1, 0, 1}, {0, 1, 0}});
    graph = Graph(adjacencyMatrix);
    assert(graph.isLimit());
    assert(graph == Graph(adjacencyMatrix));

    graph.rotateEdge(1, 2, 0);
    assert(graph == Graph({{0, 1, 1}, {1, 0, 0}, {1, 0, 0}}));

    adjacencyMatrix = vector<vector<short>>({{0, 1, 0, 0}, {1, 0, 1, 0}, {0, 1, 0, 1}, {0, 0, 1, 0}});
    graph = Graph(adjacencyMatrix);
    assert(!graph.isLimit());
    assert(!graph.isDecreasingTriple(3, 2, 0));
    assert(graph.isIncreasingTriple(3, 2, 0));

    graph.rotateEdge(3, 2, 0);
    assert(graph.isLimit());
    assert(graph.isDecreasingTriple(0, 2, 3));
    assert(!graph.isIncreasingTriple(0, 2, 3));

    graph = Graph(adjacencyMatrix);
    graph.connect(0, 2);
    assert(graph.isLimit());

    // endregion

    // region Partition

    // region Methods

    assert(Partition({3, 2, 2, 1}).rank() == 2);
    assert(Partition({5, 5, 5, 5}).rank() == 4);
    assert(!Partition({1, 2, 3, 4, 5}).isValid());

    Partition partition({7, 5, 5, 4, 1});

    assert(partition.rightmostByColumn(0) == 0);
    assert(partition.rightmostByColumn(1) == 2);
    assert(partition.rightmostByColumn(2) == 2);
    assert(partition.rightmostByColumn(3) == 3);
    assert(partition.rightmostByColumn(4) == 4);
    assert(partition.rightmostByColumn(5) == 5);

    assert(partition.rightmostByRow(0) == 4);
    assert(partition.rightmostByRow(1) == 3);
    assert(partition.rightmostByRow(2) == 3);
    assert(partition.rightmostByRow(3) == 3);
    assert(partition.rightmostByRow(4) == 2);
    assert(partition.rightmostByRow(5) == 0);
    assert(partition.rightmostByRow(6) == 0);
    assert(partition.rightmostByRow(7) == -1);

    partition = Partition({2, 1});
    partition.move(0, 2);

    assert(partition.isValid());
    assert(partition.length() == 3);
    assert(partition == Partition({1, 1, 1}));

    partition = Partition({2, 1});
    partition.insert(2);

    assert(partition.isValid());
    assert(partition.length() == 3);
    assert(partition == Partition({2, 1, 1}));

    partition = Partition({5, 3, 1, 1});

    assert(partition.rank() == 2);
    assert(partition.sum() == 10);
    assert(partition.isValid());
    assert(partition.head() == Partition({4, 2}));
    assert(partition.tail() == Partition({2}));

    partition.insert(5);
    partition.remove(5);
    partition.insert(10);
    partition.remove(10);

    partition.move(2, 3);
    assert(!partition.isValid());

    partition.move(3, 2);
    partition.move(1, 2);
    partition.remove(0);
    assert(partition.isValid());
    assert(!partition.isMaximumGraphical());

    partition.remove(0);
    assert(partition.isValid());
    assert(partition.isMaximumGraphical());

    assert(partition.head() == Partition({2, 1}));
    assert(partition.tail() == Partition({2, 1}));
    assert(partition.head() == partition.tail());

    partition.insert(1);
    assert(!partition.isMaximumGraphical());

    assert(partition.conjugate() == Partition({4, 3, 2}));

    assert(Partition({5, 4, 2, 2, 2, 1}).isGraphical());
    assert(Partition({5, 3, 2, 2, 2, 2}).isGraphical());
    assert(Partition::from(16, 1).isGraphical());
    assert(!(Partition({5, 5, 2, 2, 2}).isGraphical()));
    assert(!(Partition({6, 4, 2, 2, 1, 1}).isGraphical()));

    partition = Partition({4, 2, 2, 1, 1, 1, 1});
    Partition newTail(partition.head().conjugate());
    partition.replaceTail(newTail);

    assert(partition.isValid());
    assert(partition == Partition({4, 2, 2, 1, 1}));

    partition = Partition::from(Graph(
            {{0, 1, 0, 0},
             {1, 0, 1, 0},
             {0, 1, 0, 1},
             {0, 0, 1, 0}}
    ));

    assert(partition == Partition({2, 2, 1, 1}));

    auto isGraphical([](const Partition& p){ return p.isGraphical(); });

    partition = Partition({1, 1, 1, 1});
    vector<Partition> graphicalChildren = *partition.graphicalChildrenPtr();

    assert(partition.isGraphical());
    assert(all_of(graphicalChildren.begin(), graphicalChildren.end(), isGraphical));
    assert(graphicalChildren == vector<Partition>({Partition({2, 1, 1})}));

    partition = Partition({2, 2, 1, 1, 1, 1});
    graphicalChildren = *partition.graphicalChildrenPtr();

    assert(partition.isGraphical());
    assert(all_of(graphicalChildren.begin(), graphicalChildren.end(), isGraphical));
    assert(graphicalChildren == vector<Partition>({Partition({3, 1, 1, 1, 1, 1}), Partition({2, 2, 2, 1, 1})}));

    partition = Partition({4, 3, 1, 1, 1});
    graphicalChildren = *partition.graphicalChildrenPtr();

    assert(partition.isGraphical());
    assert(all_of(graphicalChildren.begin(), graphicalChildren.end(), isGraphical));
    assert(graphicalChildren == vector<Partition>({Partition({5, 2, 1, 1, 1}), Partition({4, 3, 2, 1})}));

    // endregion

    // region ColoredPartition

    ColoredPartition cpartition({2, 1, 1});
    cpartition.paint(BLACK, 0);
    cpartition.paint(GREY, 1);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0) == BLACK);
    assert(cpartition.getColor(1) == GREY);
    assert(cpartition.getColor(2) == NONE);
    assert(cpartition.getColor(0, 0) == NONE);
    assert(cpartition.getColor(0, 1) == BLACK);
    assert(cpartition.getColor(1, 0) == GREY);
    assert(cpartition.getColor(2, 0) == NONE);

    cpartition.remove(0);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0) == NONE);
    assert(cpartition.getColor(0, 0) == NONE);

    cpartition = ColoredPartition({2, 1});
    cpartition.paint(GREY, 0, 0);
    cpartition.paint(BLACK, 0, 1);
    cpartition.paint(GREY, 1, 0);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0, 0) == GREY);
    assert(cpartition.getColor(0, 1) == BLACK);
    assert(cpartition.getColor(1, 0) == GREY);

    cpartition = ColoredPartition({2, 1});
    cpartition.insert(2);

    assert(cpartition.isValid());
    assert(cpartition.getColor(2) == NONE);

    cpartition.paint(GREY, 2);

    assert(cpartition.isValid());
    assert(cpartition.getColor(2) == GREY);

    cpartition = ColoredPartition({2, 1});
    cpartition.insert(0);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0) == NONE);

    cpartition.paint(GREY, 0);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0) == GREY);

    cpartition = ColoredPartition({2, 1});
    cpartition.paint(BLACK, 0);
    cpartition.insert(0);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0) == NONE);

    cpartition.paint(GREY, 0);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0) == GREY);

    cpartition.remove(0);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0) == BLACK);

    cpartition = ColoredPartition({2, 1});
    cpartition.move(1, 0);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0) == NONE);

    cpartition.paint(GREY, 0);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0) == GREY);

    cpartition = ColoredPartition({2, 1});
    cpartition.paint(GREY, 1);
    cpartition.move(1, 0);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0) == GREY);

    cpartition.move(0, 1);
    cpartition.move(0, 2);

    assert(cpartition.isValid());
    assert(cpartition.getColor(0) == NONE);
    assert(cpartition.getColor(1) == GREY);
    assert(cpartition.getColor(2) == NONE);

    cpartition = ColoredPartition({2, 1});

    assert(cpartition.hasBlock(0, 0));
    assert(cpartition.hasBlock(0, 1));
    assert(!cpartition.hasBlock(0, 2));
    assert(cpartition.hasBlock(1, 0));
    assert(!cpartition.hasBlock(1, 1));
    assert(!cpartition.hasBlock(2, 1));

    cpartition = ColoredPartition({4, 2, 2, 1, 1, 1, 1});
    cpartition.paintHeadBlack();

    assert(cpartition.isValid());
    assert(cpartition.getColor(0, 0) == NONE);
    assert(cpartition.getColor(0, 1) == BLACK);
    assert(cpartition.getColor(0, 2) == BLACK);
    assert(cpartition.getColor(0, 3) == BLACK);

    assert(cpartition.getColor(1, 0) == NONE);
    assert(cpartition.getColor(1, 1) == BLACK);

    assert(cpartition.getColor(2, 0) == NONE);
    assert(cpartition.getColor(2, 1) == NONE);

    assert(cpartition.getColor(3, 0) == NONE);

    assert(cpartition.getColor(4, 0) == NONE);

    cpartition = ColoredPartition({4, 2, 2, 1, 1, 1, 1});
    cpartition.maximize();

    ColoredPartition expectedCpartition({4, 3, 2, 2, 1});
    expectedCpartition.paint(NONE, 0, 0);
    expectedCpartition.paint(BLACK, 0, 1);
    expectedCpartition.paint(BLACK, 0, 2);
    expectedCpartition.paint(BLACK, 0, 3);

    expectedCpartition.paint(NONE, 1, 0);
    expectedCpartition.paint(BLACK, 1, 1);
    expectedCpartition.paint(GREY, 1, 2);

    expectedCpartition.paint(BLACK, 2, 0);
    expectedCpartition.paint(BLACK, 2, 1);

    expectedCpartition.paint(BLACK, 3, 0);
    expectedCpartition.paint(GREY, 3, 1);

    expectedCpartition.paint(BLACK, 4, 0);

    assert(cpartition.isValid());
    assert(cpartition == expectedCpartition);

    cpartition = ColoredPartition({3, 3, 2, 1, 1, 1, 1});
    cpartition.maximize();

    expectedCpartition = ColoredPartition({4, 3, 2, 2, 1});
    expectedCpartition.paint(NONE, 0, 0);
    expectedCpartition.paint(BLACK, 0, 1);
    expectedCpartition.paint(BLACK, 0, 2);
    expectedCpartition.paint(GREY, 0, 3);

    expectedCpartition.paint(NONE, 1, 0);
    expectedCpartition.paint(BLACK, 1, 1);
    expectedCpartition.paint(BLACK, 1, 2);

    expectedCpartition.paint(BLACK, 2, 0);
    expectedCpartition.paint(BLACK, 2, 1);

    expectedCpartition.paint(BLACK, 3, 0);
    expectedCpartition.paint(BLACK, 3, 1);

    expectedCpartition.paint(GREY, 4, 0);

    assert(cpartition.isValid());
    assert(cpartition == expectedCpartition);

    // endregion

    // region Comparison

    partition = Partition({5, 3, 1, 1});

    assert(partition <= Partition({10}));

    assert(partition >= Partition({4, 4, 1, 1}));
    assert(partition >= Partition({4, 3, 2, 1}));
    assert(partition >= Partition({4, 3, 1, 1, 1}));
    assert(partition >= Partition({3, 3, 2, 1, 1}));
    assert(partition >= Partition({3, 2, 2, 1, 1, 1}));
    assert(partition >= Partition::from(10, 1));

    assert(partition >= Partition({4, 3, 1, 1}));
    assert(partition >= Partition({5, 2, 1, 1}));
    assert(partition >= Partition({5, 3, 1}));
    assert(partition >= Partition({5, 2}));
    assert(partition >= Partition({}));

    assert(!(partition >= Partition({5, 4, 1})));
    assert(!(partition >= Partition({6, 2, 1, 1})));
    assert(!(partition >= Partition({5, 3, 1, 1, 1})));
    assert(!(partition >= Partition({6, 3, 1, 1})));
    assert(!(partition >= Partition({7, 1})));

    partition = Partition({4, 2, 1, 1});

    assert(partition >= partition);
    assert(partition >= Partition({1}));
    assert(partition >= Partition({3, 1}));
    assert(partition >= Partition({2, 2, 1, 1}));
    assert(partition >= Partition({4}));
    assert(partition >= Partition({4, 1}));
    assert(partition >= Partition({4, 2}));
    assert(partition >= Partition({4, 1, 1, 1, 1}));
    assert(partition >= Partition({3, 3, 1, 1}));
    assert(partition >= Partition({2, 2, 2, 2}));
    assert(partition >= Partition({2, 1, 1, 1, 1, 1, 1}));
    assert(partition >= Partition::from(8, 1));

    assert(partition <= partition);
    assert(partition <= Partition({5, 1, 1, 1}));
    assert(partition <= Partition({4, 2, 2}));
    assert(partition <= Partition({4, 3, 1}));
    assert(partition <= Partition({5, 2, 1}));
    assert(partition <= Partition({4, 4}));
    assert(partition <= Partition({4, 5}));
    assert(partition <= Partition({5, 3}));
    assert(partition <= Partition({6, 1, 1}));
    assert(partition <= Partition({6, 2}));
    assert(partition <= Partition({7, 1}));
    assert(partition <= Partition({8}));

    assert(!(partition >= Partition({4, 3})));
    assert(!(partition >= Partition({3, 3, 2})));
    assert(!(partition >= Partition({3, 3, 2, 1})));
    assert(!(partition >= Partition({3, 3, 1, 1, 1})));
    assert(!(partition >= Partition({5})));
    assert(!(partition >= Partition({5, 1})));
    assert(!(partition >= Partition({5, 1, 1})));
    assert(!(partition >= Partition({5, 2})));
    assert(!(partition >= Partition({6})));
    assert(!(partition >= Partition({6, 1})));
    assert(!(partition >= Partition({7})));

    assert(!(partition <= Partition({4, 3})));
    assert(!(partition <= Partition({3, 3, 2})));
    assert(!(partition <= Partition({3, 3, 2, 1})));
    assert(!(partition <= Partition({3, 3, 1, 1, 1})));
    assert(!(partition <= Partition({5})));
    assert(!(partition <= Partition({5, 1})));
    assert(!(partition <= Partition({5, 1, 1})));
    assert(!(partition <= Partition({5, 2})));
    assert(!(partition <= Partition({6})));
    assert(!(partition <= Partition({6, 1})));
    assert(!(partition <= Partition({7})));

    // endregion

    // endregion

    // region Transition

    unique_ptr<PartitionTransition> transitionPtr = nullptr;

    partition = Partition({2, 1});
    transitionPtr = unique_ptr<PartitionTransition>(new PartitionMove(0, 1, 2, 0));
    transitionPtr->apply(partition);

    assert(partition.isValid());
    assert(partition.length() == 3);
    assert(partition == Partition({1, 1, 1}));

    partition = Partition({2, 1});
    transitionPtr = unique_ptr<PartitionTransition>(new PartitionInsert(1, 1));
    transitionPtr->apply(partition);

    assert(partition.isValid());
    assert(partition.length() == 2);
    assert(partition == Partition({2, 2}));

    partition = Partition({2, 1});
    transitionPtr = unique_ptr<PartitionTransition>(new PartitionInsert(2, 0));
    transitionPtr->apply(partition);

    assert(partition.isValid());
    assert(partition.length() == 3);
    assert(partition == Partition({2, 1, 1}));

    partition = Partition({2, 1});
    transitionPtr = unique_ptr<PartitionTransition>(new PartitionRemove(0, 1));
    transitionPtr->apply(partition);

    assert(partition.isValid());
    assert(partition.length() == 2);
    assert(partition == Partition({1, 1}));

    partition = Partition({2, 1});
    transitionPtr = unique_ptr<PartitionTransition>(new PartitionRemove(1, 0));
    transitionPtr->apply(partition);

    assert(partition.isValid());
    assert(partition.length() == 1);
    assert(partition == Partition({2}));

    transitionPtr = unique_ptr<PartitionTransition>(new PartitionMove(0, 2, 1, 0));

    assert(*(transitionPtr->inverse()) == PartitionMove(1, 0, 0, 2));
    assert(*(transitionPtr->inverse()) != PartitionMove(0, 1, 0, 2));
    assert(*(transitionPtr->inverse()) != PartitionInsert(0, 2));
    assert(*(transitionPtr->inverse()) != PartitionRemove(0, 2));

    transitionPtr = unique_ptr<PartitionTransition>(new PartitionInsert(0, 0));

    assert(*(transitionPtr->inverse()) == PartitionRemove(0, 0));
    assert(*(transitionPtr->inverse()) != PartitionMove(0, 1, 0, 1));
    assert(*(transitionPtr->inverse()) != PartitionInsert(0, 0));
    assert(*(transitionPtr->inverse()) != PartitionRemove(1, 1));

    transitionPtr = unique_ptr<PartitionTransition>(new PartitionRemove(0, 0));

    assert(*(transitionPtr->inverse()) == PartitionInsert(0, 0));
    assert(*(transitionPtr->inverse()) != PartitionMove(0, 1, 2, 0));
    assert(*(transitionPtr->inverse()) != PartitionInsert(1, 0));
    assert(*(transitionPtr->inverse()) != PartitionRemove(0, 0));

    transitionPtr = unique_ptr<PartitionTransition>(new PartitionMove(0, 3, 1, 2));

    assert(*(transitionPtr->conjugate()) == PartitionMove(3, 0, 2, 1));
    assert(*(transitionPtr->conjugate()) != PartitionMove(1, 2, 0, 3));
    assert(*(transitionPtr->conjugate()) != PartitionInsert(0, 2));
    assert(*(transitionPtr->conjugate()) != PartitionRemove(0, 2));

    transitionPtr = unique_ptr<PartitionTransition>(new PartitionInsert(0, 1));

    assert(*(transitionPtr->conjugate()) == PartitionInsert(1, 0));
    assert(*(transitionPtr->conjugate()) != PartitionMove(0, 1, 0, 1));
    assert(*(transitionPtr->conjugate()) != PartitionInsert(0, 0));
    assert(*(transitionPtr->conjugate()) != PartitionRemove(1, 1));

    transitionPtr = unique_ptr<PartitionTransition>(new PartitionRemove(0, 1));

    assert(*(transitionPtr->conjugate()) == PartitionRemove(1, 0));
    assert(*(transitionPtr->conjugate()) != PartitionMove(0, 1, 2, 0));
    assert(*(transitionPtr->conjugate()) != PartitionInsert(1, 0));
    assert(*(transitionPtr->conjugate()) != PartitionRemove(0, 0));

    TransitionChain chain = TransitionChain({
            new PartitionMove(0, 2, 1, 0),
            new PartitionInsert(0, 2),
            new PartitionRemove(0, 2)
    });

    assert(chain == TransitionChain(chain));

    TransitionChain expectedChain = TransitionChain({
            new PartitionInsert(0, 2),
            new PartitionRemove(0, 2),
            new PartitionMove(1, 0, 0, 2)
    });

    assert(chain.inverse() == expectedChain);

    chain = TransitionChain({
            new PartitionMove(1, 1, 0, 2),
            new PartitionInsert(0, 3),
            new PartitionRemove(0, 4)
    });

    assert(chain == TransitionChain(chain));

    expectedChain = TransitionChain({
            new PartitionMove(1, 1, 2, 0),
            new PartitionInsert(3, 0),
            new PartitionRemove(4, 0)
    });

    assert(chain.conjugate() == expectedChain);

    // endregion

    // region Algorithm

    auto actualChain = partitionTransitionChain(Partition({4, 4, 3}), Partition({6, 4, 1}));
    expectedChain = TransitionChain({new PartitionMove(2, 2, 0, 4), new PartitionMove(2, 1, 0, 5)});

    assert(actualChain == expectedChain);

    actualChain = partitionTransitionChain(Partition({2, 1, 1}), Partition({4}));
    expectedChain = TransitionChain({new PartitionMove(2, 0, 0, 2), new PartitionMove(1, 0, 0, 3)});

    assert(actualChain == expectedChain);

    actualChain = partitionTransitionChain(Partition({3, 1}), Partition({5, 1}));
    expectedChain = TransitionChain({new PartitionInsert(0, 3), new PartitionInsert(0, 4)});

    assert(actualChain == expectedChain);

    actualChain = partitionTransitionChain(Partition({3, 1, 1}), Partition({5, 3}));
    expectedChain = TransitionChain({
            new PartitionMove(2, 0, 0, 3),
            new PartitionInsert(0, 4), new PartitionInsert(1, 1), new PartitionInsert(1, 2)
    });

    assert(actualChain == expectedChain);

    actualChain = partitionTransitionChain(Partition({2, 2}), Partition({5, 1}));
    expectedChain = TransitionChain({
            new PartitionMove(1, 1, 0, 2),
            new PartitionInsert(0, 3), new PartitionInsert(0, 4)
    });

    assert(actualChain == expectedChain);

    partition = Partition({4, 2, 2, 1, 1, 1, 1});
    Partition headConjugate = partition.head().conjugate();
    Partition tailConjugate = partition.tail().conjugate();
    headTailConjugateChain(partition).apply(headConjugate);

    //cout << partition.head() << endl;
    //cout << partition.tail() << endl;
    //cout << partition.head().conjugate() << endl;
    //cout << partition.tail().conjugate() << endl;
    //cout << headTailConjugateChain(partition) << endl;
    //cout << headConjugate << endl;
    //cout << tailConjugate << endl;

    assert(headConjugate == tailConjugate);

    partition = Partition({3, 3, 2, 1, 1, 1, 1});
    headConjugate = partition.head().conjugate();
    tailConjugate = partition.tail().conjugate();
    headTailConjugateChain(partition).apply(headConjugate);

    assert(headConjugate == tailConjugate);

    partition = Partition({4, 2, 2, 1, 1, 1, 1});
    actualChain = inverseGraphicallyMaximizingChain(partition).inverse();
    expectedChain = TransitionChain({
            new PartitionMove(6, 0, 1, 2),
            new PartitionMove(5, 0, 3, 1)
    });

    assert(actualChain == expectedChain);

    actualChain.apply(partition);

    assert(partition.isMaximumGraphical());

    partition = Partition({3, 3, 2, 1, 1, 1, 1});
    actualChain = inverseGraphicallyMaximizingChain(partition).inverse();
    expectedChain = TransitionChain({
            new PartitionMove(6, 0, 0, 3),
            new PartitionMove(5, 0, 3, 1)
    });

    assert(actualChain == expectedChain);

    actualChain.apply(partition);

    assert(partition.isMaximumGraphical());

    partition = Partition({});
    actualChain = inverseGraphicallyMaximizingChain(partition).inverse();
    expectedChain = TransitionChain(vector<PartitionTransition*>());

    assert(actualChain == expectedChain);

    actualChain.apply(partition);

    assert(partition.isMaximumGraphical());

    // endregion

    cout << "Tests passed" << endl;
}

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
    test();

    //graphMain(argc, argv);
    partitionMain(argc, argv);

    return 0;
}