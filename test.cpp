#include "test.hpp"



void assertIgnore(bool value) {
    //cout << "YOU HAVE IGNORED TEST" << endl;
}

void LimitGraphTest::all() {
    LimitGraphTest::partition();
    LimitGraphTest::graph();
    LimitGraphTest::transition();
    LimitGraphTest::algorithm();
}

void LimitGraphTest::partition() {

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
    vector<Partition> actual;
    vector<Partition> expected;
    vector<Partition> difference;

    partition = Partition({2, 2, 1, 1, 1, 1});
    actual.clear();
    partitionGraphicalAscendants(partition, actual);
    expected = {Partition({3, 1, 1, 1, 1, 1}), Partition({3, 2, 1, 1, 1}), Partition({2, 2, 2, 1, 1})};
    difference.clear();

    set_symmetric_difference(
            actual.begin(),
            actual.end(),
            expected.begin(),
            expected.end(),
            inserter(difference, difference.begin())
    );

    assert(partition.isGraphical());
    assert(all_of(actual.begin(), actual.end(), isGraphical));
    assert(difference.empty());


    partition = Partition({1, 1, 1, 1});
    actual.clear();
    partitionBasicGraphicalAscendants(partition, actual);
    expected = {Partition({2, 1, 1})};
    difference.clear();

    set_symmetric_difference(
            actual.begin(),
            actual.end(),
            expected.begin(),
            expected.end(),
            inserter(difference, difference.begin())
    );

    assert(partition.isGraphical());
    assert(all_of(actual.begin(), actual.end(), isGraphical));
    assert(difference.empty());


    partition = Partition({2, 2, 1, 1, 1, 1});
    actual.clear();
    partitionBasicGraphicalAscendants(partition, actual);
    expected = {Partition({3, 1, 1, 1, 1, 1}), Partition({2, 2, 2, 1, 1})};
    difference.clear();

    set_symmetric_difference(
            actual.begin(),
            actual.end(),
            expected.begin(),
            expected.end(),
            inserter(difference, difference.begin())
    );

    assert(partition.isGraphical());
    assert(all_of(actual.begin(), actual.end(), isGraphical));
    assert(difference.empty());


    partition = Partition({4, 3, 2, 2, 1});
    actual.clear();
    partitionBasicGraphicalAscendants(partition, actual);
    expected = {};
    difference.clear();

    set_symmetric_difference(
            actual.begin(),
            actual.end(),
            expected.begin(),
            expected.end(),
            inserter(difference, difference.begin())
    );

    assert(partition.isGraphical());
    assert(all_of(actual.begin(), actual.end(), isGraphical));
    assert(difference.empty());


    partition = Partition({4, 3, 2, 2, 2, 2, 1});
    actual.clear();
    partitionBasicGraphicalAscendants(partition, actual);
    expected = {Partition({4, 3, 3, 2, 2, 2}), Partition({4, 3, 3, 2, 2, 1, 1}), Partition({5, 2, 2, 2, 2, 2, 1})};
    difference.clear();

    set_symmetric_difference(
            actual.begin(),
            actual.end(),
            expected.begin(),
            expected.end(),
            inserter(difference, difference.begin())
    );

    assert(partition.isGraphical());
    assert(all_of(actual.begin(), actual.end(), isGraphical));
    assert(difference.empty());


    partition = Partition({5, 4, 3, 3, 3, 2, 2});
    actual.clear();
    partitionBasicGraphicalAscendants(partition, actual);
    expected = {Partition({6, 3, 3, 3, 3, 2, 2}), Partition({5, 4, 4, 3, 2, 2, 2}), Partition({5, 4, 3, 3, 3, 3, 1})};
    difference.clear();

    set_symmetric_difference(
            actual.begin(),
            actual.end(),
            expected.begin(),
            expected.end(),
            inserter(difference, difference.begin())
    );

    assert(partition.isGraphical());
    assert(all_of(actual.begin(), actual.end(), isGraphical));
    assert(difference.empty());

    partition = Partition({4, 1, 1, 1, 1, 1, 1});
    actual.clear();
    partitionBasicGraphicalAscendants(partition, actual);
    expected = {Partition({4, 2, 1, 1, 1, 1})};
    difference.clear();

    set_symmetric_difference(
            actual.begin(),
            actual.end(),
            expected.begin(),
            expected.end(),
            inserter(difference, difference.begin())
    );

    assert(partition.isGraphical());
    assert(all_of(actual.begin(), actual.end(), isGraphical));
    assert(difference.empty());

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

}

void LimitGraphTest::graph() {
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
}

void LimitGraphTest::transition() {
    unique_ptr<PartitionTransition> transitionPtr = nullptr;

    Partition partition = Partition({2, 1});
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
}

void LimitGraphTest::algorithm() {
    // region Chains

    auto actualChain = partitionTransitionChain(Partition({4, 4, 3}), Partition({6, 4, 1}));
    TransitionChain expectedChain = TransitionChain({new PartitionMove(2, 2, 0, 4), new PartitionMove(2, 1, 0, 5)});

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

    Partition partition = Partition({4, 2, 2, 1, 1, 1, 1});
    Partition headConjugate = partition.head().conjugate();
    Partition tailConjugate = partition.tail().conjugate();
    headTailConjugateChain(partition).apply(headConjugate);

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

    unique_ptr<deque<Partition>> actualPartitionChainPtr;
    deque<Partition> expectedPartitionChain;

    // Partition 1x10
    actualPartitionChainPtr = findShortestMaximizingChainPtr(Partition({1, 1, 1, 1, 1, 1, 1, 1, 1, 1}));
    expectedPartitionChain = {
            Partition({1, 1, 1, 1, 1, 1, 1, 1, 1, 1}),
            Partition({2, 1, 1, 1, 1, 1, 1, 1, 1}),
            Partition({3, 1, 1, 1, 1, 1, 1, 1,}),
            Partition({4, 1, 1, 1, 1, 1, 1}),
            Partition({5, 1, 1, 1, 1, 1}),
    };

    //cout << "Actual partition chain: " << endl << *actualPartitionChainPtr << endl;
    //cout << "Expected partition chain: " << endl << expectedPartitionChain << endl;

    assert(*actualPartitionChainPtr == expectedPartitionChain);

    actualPartitionChainPtr = findShortestMaximizingChainPtr(Partition({4, 2, 2, 1, 1, 1, 1}));
    expectedPartitionChain = {
            Partition({4, 2, 2, 1, 1, 1, 1}),
            Partition({4, 3, 2, 1, 1, 1}),
            Partition({4, 3, 2, 2, 1})
    };

    //cout << "Actual partition chain: " << endl << *actualPartitionChainPtr << endl;
    //cout << "Expected partition chain: " << endl << expectedPartitionChain << endl;

    assertIgnore(*actualPartitionChainPtr == expectedPartitionChain);

    actualPartitionChainPtr = findShortestMaximizingChainPtr(Partition({3, 3, 2, 1, 1, 1, 1}));
    expectedPartitionChain = {
            Partition({3, 3, 2, 1, 1, 1, 1}),
            Partition({4, 3, 2, 1, 1, 1}),
            Partition({4, 3, 2, 2, 1})
    };

    //cout << "Actual partition chain: " << endl << *actualPartitionChainPtr << endl;
    //cout << "Expected partition chain: " << endl << expectedPartitionChain << endl;

    assertIgnore(*actualPartitionChainPtr == expectedPartitionChain);

    // endregion

    //region Search

    vector<Partition> difference;

    // MGP1
    unordered_set<Partition> actualMGPsPtr = *findMaximumGraphicalPartitionsPtr(Partition({4, 2, 1, 1, 1, 1}));
    vector<Partition> expectedMGPsPtr = {
            Partition({5, 1, 1, 1, 1, 1}),
            Partition({4, 2, 2, 1, 1})
    };

    set_symmetric_difference(
            actualMGPsPtr.begin(),
            actualMGPsPtr.end(),
            expectedMGPsPtr.begin(),
            expectedMGPsPtr.end(),
            inserter(difference, difference.begin())
    );

    assert(difference.empty());

    // MGP2
    actualMGPsPtr = *findMaximumGraphicalPartitionsPtr(Partition({3, 3, 2, 1, 1}));
    expectedMGPsPtr = {
            Partition({4, 2, 2, 1, 1}),
            Partition({3, 3, 2, 2})
    };

    set_symmetric_difference(
            actualMGPsPtr.begin(),
            actualMGPsPtr.end(),
            expectedMGPsPtr.begin(),
            expectedMGPsPtr.end(),
            inserter(difference, difference.begin())
    );

    assert(difference.empty());

    // MGP3
    actualMGPsPtr = *findMaximumGraphicalPartitionsPtr(Partition({3, 3, 2, 2}));
    expectedMGPsPtr = {Partition({3, 3, 2, 2})};

    set_symmetric_difference(
            actualMGPsPtr.begin(),
            actualMGPsPtr.end(),
            expectedMGPsPtr.begin(),
            expectedMGPsPtr.end(),
            inserter(difference, difference.begin())
    );

    assert(difference.empty());

    // MGP4
    actualMGPsPtr = *findMaximumGraphicalPartitionsPtr(Partition({3, 3, 1, 1, 1, 1}));
    expectedMGPsPtr = {
            Partition({5, 1, 1, 1, 1, 1}),
            Partition({4, 2, 2, 1, 1}),
            Partition({3, 3, 2, 2})
    };

    set_symmetric_difference(
            actualMGPsPtr.begin(),
            actualMGPsPtr.end(),
            expectedMGPsPtr.begin(),
            expectedMGPsPtr.end(),
            inserter(difference, difference.begin())
    );

    assert(difference.empty());

    // MGP5
    actualMGPsPtr = *findMaximumGraphicalPartitionsPtr(Partition({1, 1, 1, 1, 1, 1, 1, 1, 1, 1}));
    expectedMGPsPtr = {
            Partition({5, 1, 1, 1, 1, 1}),
            Partition({4, 2, 2, 1, 1}),
            Partition({3, 3, 2, 2})
    };

    set_symmetric_difference(
            actualMGPsPtr.begin(),
            actualMGPsPtr.end(),
            expectedMGPsPtr.begin(),
            expectedMGPsPtr.end(),
            inserter(difference, difference.begin())
    );

    assert(difference.empty());

    // MGP6
    actualMGPsPtr = *findMaximumGraphicalPartitionsPtr(Partition({4}));

    assert(actualMGPsPtr.size() == 0);

    // PSA1
    difference.clear();
    partition = Partition({3, 2, 1, 1, 1, 1, 1});
    PartitionSearchAlgorithm partitionSearchAlgorithm(partition);
    vector<Partition> actualPartitions = *partitionSearchAlgorithm.getPartitions();
    vector<int> actualDistances = *partitionSearchAlgorithm.getDistances();
    vector<Partition> expectedPartitions = vector<Partition>({
          Partition({5,1,1,1,1,1}),
          Partition({4,2,2,1,1}),
          Partition({3,3,2,2})
    });
    vector<int> expectedDistances = vector<int>({ 3,3,3 });

    set_symmetric_difference(
            actualPartitions.begin(),
            actualPartitions.end(),
            expectedPartitions.begin(),
            expectedPartitions.end(),
            inserter(difference, difference.begin())
    );

    assert(difference.empty());
    assert(expectedDistances.size() == actualDistances.size());

    for (int i = 0; i < actualPartitions.size(); i++) {
        for (int j = 0; j < expectedPartitions.size(); j++) {
            if (expectedPartitions[i] == actualPartitions[j]) {
                assert(actualDistances[i] == expectedDistances[j]);
            }
        }
    }

    // PSA2
    difference.clear();
    partition = Partition({3,3,2,1,1});
    partitionSearchAlgorithm = PartitionSearchAlgorithm(partition);
    actualPartitions = *partitionSearchAlgorithm.getPartitions();
    actualDistances = *partitionSearchAlgorithm.getDistances();
    expectedPartitions = vector<Partition>({
          Partition({4,2,2,1,1}),
          Partition({3,3,2,2})
    });
    expectedDistances = vector<int>({ 1,1 });

    set_symmetric_difference(
            actualPartitions.begin(),
            actualPartitions.end(),
            expectedPartitions.begin(),
            expectedPartitions.end(),
            inserter(difference, difference.begin())
    );

    assert(difference.empty());
    assert(expectedDistances.size() == actualDistances.size());

    for (int i = 0; i < actualPartitions.size(); i++) {
        for (int j = 0; j < expectedPartitions.size(); j++) {
            if (expectedPartitions[i] == actualPartitions[j]) {
                assert(actualDistances[i] == expectedDistances[j]);
            }
        }
    }

    //endregion
}
//TODO: Replace commented prints with logger

ostream& operator<<(ostream& strm, vector<int> numbers) {
    for_each(numbers.begin(), numbers.end(), [&strm](const int& n){strm << n << " ";});
    //for_each(partitionChain.begin(), partitionChain.end(), [&strm](const Partition& p){ strm << p.toString() << endl; });
    //for (int i = 0; i < numbers.size(); i++)
    return strm;
}
