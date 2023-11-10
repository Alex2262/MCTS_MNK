//
// Created by Alexander Tian on 5/22/23.
//

#ifndef MCTS_MNK_POSITION_H
#define MCTS_MNK_POSITION_H

#include "constants.h"
#include <vector>
#include <unordered_set>

struct Threats {
    std::unordered_set<Move> our_threats_1{};  // One move threats
    std::unordered_set<Move> our_threats_2{};  // Threats to create a chain with threats on both sides

    std::unordered_set<Move> opp_threats_1{};  // One move threats
    std::unordered_set<Move> opp_threats_2{};  // Threats to create a chain with threats on both sides
};

class Position {
public:

    void make_move(Move move);
    std::vector<Move> get_moves();
    std::vector<Move> get_adjacent_moves(int adjacency_range);

    void ray_threats(Threats& threats, int color, uint16_t row, uint16_t col, Increment increment);
    void get_square_threats(Threats& threats, uint16_t row, uint16_t col);
    void get_threats(Threats& threats);

    int get_result(Move last_move);
    void print_board();
    void visualize_moves(const std::vector<Move>& moves);

    int side = 0;

    int board[BOARD_HEIGHT][BOARD_WIDTH]{};

    Position() {
        for (auto & i : board) {
            for (int & j : i) {
                j = EMPTY;
            }
        }
    }
};


#endif //MCTS_MNK_POSITION_H
