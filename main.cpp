#include <iostream>
#include "mcts.h"
#include "perft.h"


int main() {
    MCTS mcts{};
    mcts.tree.graph.emplace_back(0, NO_MOVE);

    mcts.position.print_board();

    std::string msg;
    while (getline(std::cin, msg)) {

        std::vector <std::string> tokens = split(msg, ' ');

        if (tokens[0] == "perft") {
            std::cout << perft(mcts.position, 4);
        }

        if (tokens[0] == "go") {
            auto time = std::chrono::high_resolution_clock::now();
            uint64_t current_time = std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::time_point_cast<std::chrono::milliseconds>(time).time_since_epoch()).count();

            mcts.start_time = current_time;
            uint64_t best_node_index = mcts.search();
            Node& best_node = mcts.tree.graph[best_node_index];

            double win_probability = mcts.get_win_probability(best_node_index);

            std::string win_probability_color = win_probability >  30 ? GREEN :
                                                win_probability < -30 ? RED   :
                                                YELLOW;

            time = std::chrono::high_resolution_clock::now();
            current_time = std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::time_point_cast<std::chrono::milliseconds>(time).time_since_epoch()).count();

            uint64_t elapsed_time = current_time - mcts.start_time;

            std::cout << std::endl
                      << "Total Iterations: \t" << CYAN << mcts.iterations << RESET << "\n"
                      << "Score: \t\t\t\t"      << CYAN << best_node.win_count << RESET << "\n"
                      << "Visits: \t\t\t"       << CYAN << best_node.visits << RESET << "\n"
                      << "Confidence: \t\t"     << win_probability_color << win_probability << "%\n" << RESET
                      << "Seldepth: \t\t\t"     << CYAN << mcts.seldepth << RESET << "\n"
                      << "Time: \t\t\t\t"       << CYAN << elapsed_time << RESET << "\n"
                      << "IPS: \t\t\t\t"        << CYAN << mcts.iterations / (elapsed_time / 1000) << RESET
                      << std::endl << std::endl;

            Move best_move = best_node.last_move;

            std::cout << "best move [" << CYAN << best_move.row << ", " << best_move.col << RESET << "]"
                      << std::endl << std::endl;

            mcts.position.make_move<MOVE_ADJACENCY>(best_move);
            mcts.position.print_board();

            mcts.root_node_index = best_node_index;

            mcts.flatten_tree();
        }

        if (tokens[0] == "move") {
            Move sent_move = Move{static_cast<uint16_t>(std::stoi(tokens[1])),
                                  static_cast<uint16_t>(std::stoi(tokens[2]))};
            mcts.position.make_move<MOVE_ADJACENCY>(sent_move);
            std::cout << std::endl;

            mcts.position.print_board();

            int node_index = -1;
            for (int i = 0; i < mcts.tree.graph[mcts.root_node_index].children_end - mcts.tree.graph[mcts.root_node_index].children_start; i++) {
                Move& match_move = mcts.tree.graph[mcts.tree.graph[mcts.root_node_index].children_start + i].last_move;
                if (sent_move.col == match_move.col && sent_move.row == match_move.row) {
                    node_index = i;
                    break;
                }
            }

            if (node_index == -1) {
                mcts.tree.graph.emplace_back(mcts.root_node_index, sent_move);
                mcts.tree.graph[mcts.root_node_index].children_end = mcts.tree.graph.size();
                mcts.root_node_index = mcts.tree.graph.size() - 1;
            } else {
                mcts.root_node_index = mcts.tree.graph[mcts.root_node_index].children_start + node_index;
            }

            mcts.flatten_tree();
        }

        if (tokens[0] == "help") {
            std::cout << "Type go for the MCTS engine to make a move\n";
            std::cout << "Type move {row} {col} to make a move\n";
        }

        int result = mcts.position.get_result(mcts.tree.graph[mcts.root_node_index].last_move);
        if (result != NO_SCORE) {
            std::cout << "Result: " << result << std::endl;
            break;
        }

        if (mcts.position.get_moves().empty()) {
            std::cout << "DRAW" << std::endl;
            break;
        }
    }

    return 0;
}
