//
// Created by Alexander Tian on 5/22/23.
//

#ifndef MCTS_MNK_MCTS_H
#define MCTS_MNK_MCTS_H

#include <thread>
#include "constants.h"
#include "position.h"

class Node {
public:
    uint32_t parent = 0;
    uint32_t children_start = 0;
    uint32_t children_end = 0;
    int win_count = 0;
    int visits = 1;
    Move last_move;

    Node(uint32_t c_parent, Move c_last_move) {
        parent = c_parent;
        win_count = 0;
        visits = 1;
        last_move = c_last_move;
    }
};


class Tree {
public:
    std::vector<Node> graph{};
};

class MCTS {
public:
    MCTS() = default;

    Threats current_threats{};

    Position position{};
    Position test_position{};

    uint64_t start_time = 0;
    PLY_TYPE seldepth = 0;
    int iterations = 0;
    int simulation_results[THREADS]{};
    std::thread simulation_threads[THREADS]{};

    uint32_t root_node_index = 0;

    Tree tree{};

    double get_win_probability(uint32_t node_index);
    uint32_t select_best_child(uint32_t node_index);
    uint32_t selection();
    void expansion(uint32_t node_index);
    void simulation(uint32_t node_index, int thread_id);
    void back_propagation(uint32_t node_index, int result);
    uint32_t get_best_node();
    uint32_t search();

    void flatten_tree();
};


#endif //MCTS_MNK_MCTS_H
