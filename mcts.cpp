//
// Created by Alexander Tian on 5/22/23.
//

#include <iostream>
#include <cmath>
#include <queue>
#include "mcts.h"



double MCTS::get_win_probability(uint32_t node_index) {
    double win_ratio = static_cast<double>(tree.graph[node_index].win_count) / tree.graph[node_index].total_nodes;
    int win_side = (win_ratio > 0) - (win_ratio < 0);

    win_ratio = win_side * (std::pow(abs(win_ratio) + 0.03, 0.71) - 0.1 + 0.2 * abs(win_ratio));

    double win_probability = std::max(-1.0, std::min(1.0, win_ratio)) * 100.0;
    return win_probability;
}


/*
 * UCT FORMULA
double exploitation_value = double(child_node.win_count) / child_node.total_nodes;
double exploration_value = sqrt(std::log(tree.graph[current_node_index].total_nodes)
        / child_node.total_nodes);

double uct_value = exploitation_value + EXPLORATION_CONSTANT * exploration_value;

*/

uint32_t MCTS::select_best_child(uint32_t node_index) {

    Node node = tree.graph[node_index];

    uint32_t n_children = tree.graph[node_index].children_end - tree.graph[node_index].children_start;

    std::vector<double> policies(n_children);
    test_position.get_threats(current_threats);

    double max_policy = 0;
    for (int i = 0; i < n_children; i++) {
        uint32_t child_node_index = tree.graph[node_index].children_start + i;
        Node child_node = tree.graph[child_node_index];

        double policy;

        int distance_range = WIN_AMT - 1;

        double best_distance = std::max(BOARD_WIDTH, BOARD_HEIGHT) + 2;
        int near_stones = 0;

        for (int r = -distance_range; r <= distance_range; r++) {
            for (int c = -distance_range; c <= distance_range; c++) {

                int new_row = r + child_node.last_move.row;
                int new_col = c + child_node.last_move.col;
                if (new_row < 0 || new_row >= BOARD_HEIGHT || new_col < 0 || new_col >= BOARD_WIDTH) continue;
                if (new_row == child_node.last_move.row && new_col == child_node.last_move.col) continue;

                if (test_position.board[new_row][new_col] != EMPTY) {
                    int current_distance = std::max(abs(r), abs(c));
                    if (current_distance < best_distance) {
                        best_distance = current_distance;
                    }

                    near_stones++;
                }
            }
        }

        best_distance = std::max(best_distance, 1.7);


        policy = near_stones == 0 ? 1.0 :
                 30 +
                 near_stones / 4.0 +
                 (distance_range - best_distance) * 10 +
                         (current_threats.our_threats_1.find(child_node.last_move) != current_threats.our_threats_1.end() ? 1500 : 0) +
                         (current_threats.opp_threats_1.find(child_node.last_move) != current_threats.opp_threats_1.end() ? 800 : 0) +
                         (current_threats.our_threats_2.find(child_node.last_move) != current_threats.our_threats_2.end() ? 150 : 0) +
                         (current_threats.opp_threats_2.find(child_node.last_move) != current_threats.opp_threats_2.end() ? 80 : 0);

        policies[i] = policy;
        if (policy > max_policy) {
            max_policy = policy;
        }
    }

    // Normalize policies to [0.0, 1.0]
    for (double& policy : policies) {
        policy /= max_policy;
    }

    uint32_t best_node_index = 0;
    double best_puct = -1000000;

    for (int i = 0; i < n_children; i++) {
        uint32_t child_node_index = tree.graph[node_index].children_start + i;
        Node child_node = tree.graph[child_node_index];

        double exploitation_value = static_cast<double>(child_node.win_count) / static_cast<double>(child_node.total_nodes);
        double exploration_value = EXPLORATION_CONSTANT * std::sqrt(std::log(node.total_nodes) / child_node.total_nodes);

        double puct = exploitation_value + exploration_value * policies[i];

        if (puct > best_puct) {
            best_puct = puct;
            best_node_index = child_node_index;
        }
    }

    return best_node_index;
}

uint32_t MCTS::selection() {
    uint32_t leaf_node_index = root_node_index;
    test_position = position;

    int depth = 0;
    while (true) {

        if (tree.graph[leaf_node_index].children_end <= tree.graph[leaf_node_index].children_start) break;

        leaf_node_index = select_best_child(leaf_node_index);

        test_position.make_move(tree.graph[leaf_node_index].last_move);

        depth++;

    }

    seldepth = std::max<PLY_TYPE>(seldepth, depth);
    return leaf_node_index;
}

void MCTS::expansion(uint32_t node_index) {
    std::vector<Move> moves = test_position.get_moves();
    tree.graph[node_index].children_start = tree.graph.size();
    for (Move move : moves) {
        tree.graph.emplace_back(node_index, move);
    }
    tree.graph[node_index].children_end = tree.graph.size();
}

int MCTS::simulation(uint32_t node_index) {
    Position current_board = test_position;

    Move last_move = tree.graph[node_index].last_move;

    int current_result = NO_SCORE;
    for (int depth = 0; depth < MAX_SIMULATION_DEPTH; depth++) {

        current_result = current_board.get_result(last_move);
        if (current_result != NO_SCORE) break;

        int adjacency_range = 1;

        Threats threats{};
        current_board.get_threats(threats);
        last_move = !threats.our_threats_1.empty() ? *threats.our_threats_1.begin() :
                    !threats.opp_threats_1.empty() ? *threats.opp_threats_1.begin() :
                    !threats.our_threats_2.empty() ? *threats.our_threats_2.begin() :
                    !threats.opp_threats_2.empty() ? *threats.opp_threats_2.begin() :
                    NO_MOVE;

        if (last_move.row == BOARD_HEIGHT && last_move.col == BOARD_WIDTH) {
            std::vector<Move> moves = current_board.get_adjacent_moves(adjacency_range);
            if (moves.empty()) {
                current_result = DRAW_SCORE;
                break;
            }
            last_move = moves[rand() % moves.size()];
        }

        current_board.make_move(last_move);
    }

    if (current_result == NO_SCORE) current_result = DRAW_SCORE;

    return current_result;
}

void MCTS::back_propagation(uint32_t node_index, int result) {

    uint32_t current_node_index = node_index;
    int current_side = test_position.side ^ 1;
    while (true) {
        Node& current_node = tree.graph[current_node_index];

        current_node.total_nodes++;
        if (current_side == result) current_node.win_count++;
        else if ((current_side ^ 1) == result) current_node.win_count--;

        if (current_node.parent == current_node_index) break;  // Hit root

        current_node_index = current_node.parent;
        current_side ^= 1;
    }

    // for (uint64_t child_node_index : tree.graph[node_index].children) {
    //     assert(tree.graph[child_node_index].own_index == child_node_index);
    // }
}

uint32_t MCTS::get_best_node() {
    int best = -1;
    uint32_t best_index = 0;
    for (int i = 0; i < tree.graph[root_node_index].children_end - tree.graph[root_node_index].children_start; i++) {
        Node& node = tree.graph[tree.graph[root_node_index].children_start + i];
        if (node.total_nodes >= best) {
            best = node.total_nodes;
            best_index = tree.graph[root_node_index].children_start + i;
        }
    }

    return best_index;
}

uint32_t MCTS::search() {
    seldepth = 0;
    iterations = 0;
    test_position = position;

    for (int iteration = 0; iteration < MAX_ITERATIONS; iteration++) {
        iterations = iteration;

        uint32_t selected_node_index = selection();

        tree.graph[selected_node_index].visits++;

        int node_result = test_position.get_result(tree.graph[selected_node_index].last_move);
        if (node_result == NO_SCORE && tree.graph[selected_node_index].visits >= 2) {

            expansion(selected_node_index);

            if (tree.graph[selected_node_index].children_end > tree.graph[selected_node_index].children_start) {
                int random_index = rand() % (tree.graph[selected_node_index].children_end - tree.graph[selected_node_index].children_start);
                selected_node_index = tree.graph[selected_node_index].children_start + random_index;
                test_position.make_move(tree.graph[selected_node_index].last_move);
            }
        }

        int simulation_result = node_result == NO_SCORE ? simulation(selected_node_index) : node_result;

        back_propagation(selected_node_index, simulation_result);

        if ((iteration & 1023) == 0) {
            auto time = std::chrono::high_resolution_clock::now();
            uint64_t current_time = std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::time_point_cast<std::chrono::milliseconds>(time).time_since_epoch()).count();

            if (current_time - start_time >= MAX_TIME) {
                break;
            }
        }

        if ((iteration % 1000) == 0 && iteration != 0) {
            uint64_t best_node_index = get_best_node();
            double win_probability = get_win_probability(best_node_index);
            std::string win_probability_color = win_probability >  30 ? GREEN :
                                                win_probability < -30 ? RED   :
                                                YELLOW;
            std::cout << "\riteration [" << CYAN << iteration << RESET << "]"
                      << " depth ["      << CYAN << seldepth  << RESET << "]"
                      << " pv ["         << CYAN
                      << tree.graph[best_node_index].last_move.row << ", "
                      << tree.graph[best_node_index].last_move.col
                      << RESET << "]"
                      << " confidence [" << win_probability_color << win_probability << "%" << RESET << "]"
                      << std::flush;
        }
    }

    uint64_t best_node_index = get_best_node();
    std::cout << std::endl;

    return best_node_index;
}

void MCTS::flatten_tree() {
    std::vector copy_graph = tree.graph;

    auto start_size = tree.graph.size();

    tree.graph.clear();

    std::queue<std::pair<uint32_t, uint32_t>> next_nodes_index;
    next_nodes_index.push({root_node_index, 0});
    tree.graph.push_back(copy_graph[root_node_index]);

    root_node_index = 0;

    tree.graph[0].parent = 0;

    while (!next_nodes_index.empty()) {
        auto current_node_index = next_nodes_index.front();
        uint32_t old_node_index = current_node_index.first;
        uint32_t new_node_index = current_node_index.second;

        Node& current_old_node = copy_graph[old_node_index];
        Node& current_new_node = tree.graph[new_node_index];

        current_new_node.children_start = tree.graph.size();
        for (int i = 0; i < current_old_node.children_end - current_old_node.children_start; i++) {
            tree.graph.push_back(copy_graph[current_old_node.children_start + i]);
            tree.graph[tree.graph.size() - 1].parent = new_node_index;

            next_nodes_index.push({current_old_node.children_start + i, tree.graph.size() - 1});
        }

        current_new_node.children_end = tree.graph.size();

        next_nodes_index.pop();
    }

    /*
    std::queue<uint32_t> new_node_index;
    new_node_index.push(root_node_index);
    while (!new_node_index.empty()) {
        uint32_t current_node_index = new_node_index.front();
        Node& current_node = tree.graph[current_node_index];

        std::cout << "Current node index: " << current_node_index << std::endl;
        for (int i = 0; i < current_node.children_end - current_node.children_start; i++) {
            std::cout << current_node.children_start + i << " " << tree.graph[current_node.children_start + i].parent << std::endl;
        }

        new_node_index.pop();
    }
    */

    std::cout << "Tree flattened from " << start_size << " to " << tree.graph.size() << std::endl;
}