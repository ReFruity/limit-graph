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

unique_ptr<Graph> randomGraphPtr(unsigned int size) {
    vector<vector<short>> adjacencyMatrix(size, vector<short>(size, 0));

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < i; j++) {
            auto randomNum = (short) (rand() % 2);
            adjacencyMatrix[i][j] = adjacencyMatrix[j][i] = randomNum;
        }
    }

    return unique_ptr<Graph>(new Graph(adjacencyMatrix));
}

unique_ptr<Partition> randomGraphPartitionPtr(unsigned int graphSize) {
    return unique_ptr<Partition>(new Partition(Partition::from(*randomGraphPtr(graphSize))));
}

unique_ptr<Partition> randomPartitionPtr(unsigned int sum){
    auto length = (unsigned int) (sqrt(2 * sum) + 1);
    vector<vector<short>> matrix(length, vector<short>(length, 0));

    while (sum > 0) {
        int randX = rand() % length;
        int randY = rand() % length;

        if (matrix[randX][randY] == 0) {
            matrix[randX][randY] = 1;
            sum--;
        }
    }

    for (int i = 0; i < length; i++) {
        for (int j = 1; j < length; j++) {
            matrix[i][0] += matrix[i][j];
        }
    }

    vector<unsigned int> accumulated;
    accumulated.reserve(length);

    for (int i = 0; i < length; i++) {
        accumulated.push_back(matrix[i][0]);
    }

    sort(accumulated.begin(), accumulated.end(), greater<unsigned int>());

    return unique_ptr<Partition>(new Partition(accumulated));
}

// Uses non-basic block movements, take care
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

// TODO: This method has already invalidated hypothesis, remove?
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

// TODO: output as input? seriously?
// TODO: Also consider renaming descendants
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

        for (const auto& child: graphicalAscendants) {
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

unique_ptr<unordered_set<Partition>> findMaximumGraphicalPartitionsPtr(const Partition& startPartition) {
    deque<Partition> queue({startPartition});
    unique_ptr<unordered_set<Partition>> result(new unordered_set<Partition>());
    unordered_set<Partition> visited;

    while (!queue.empty()) {
        Partition partition = queue.front();
        queue.pop_front();

        auto alreadyFound = result->count(partition) > 0;
        if (partition.isMaximumGraphical() && !alreadyFound) {
            result->insert(partition);
            continue;
        }

        vector<Partition> graphicalAscendants;
        partitionGraphicalAscendants(partition, graphicalAscendants);

        for (const auto& child: graphicalAscendants) {
            if (visited.count(child) > 0) {
                continue;
            }

            queue.push_back(child);
        }

        visited.insert(partition);
    }

    return result;
}

PartitionSearchAlgorithm::PartitionSearchAlgorithm(const Partition& graphicalPartition) : partition(graphicalPartition) {
    deque<Partition> queue({graphicalPartition});
    unordered_set<Partition> visited;
    vector<Partition> basicGraphicalAscendants;
    unordered_map<Partition, Partition> parent;

    while(!queue.empty()) {
        Partition frontPartition = queue.front();
        queue.pop_front();

        if (visited.count(frontPartition) > 0) {
            continue;
        }

        if (frontPartition.isMaximumGraphical()) {
            this->partitions.push_back(frontPartition);
        }

        visited.insert(frontPartition);
        basicGraphicalAscendants.clear();
        partitionBasicGraphicalAscendants(frontPartition, basicGraphicalAscendants);

        for (const auto& child: basicGraphicalAscendants) {
            queue.push_back(child);
            parent.insert({child, frontPartition});
        }
    }

    for (auto currentPartition: this->partitions) {
        int distance = 0;
        while (currentPartition != graphicalPartition) {
            currentPartition = parent.at(currentPartition);
            distance++;
        }
        this->distances.push_back(distance);
    }
}

unique_ptr<vector<Partition>> PartitionSearchAlgorithm::getPartitions() {
    return unique_ptr<vector<Partition>>(new vector<Partition>(partitions));
}

unique_ptr<vector<int>> PartitionSearchAlgorithm::getDistances() {
    return unique_ptr<vector<int>>(new vector<int>(distances));
}
