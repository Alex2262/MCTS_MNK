//
// Created by Alexander Tian on 5/22/23.
//

#ifndef MCTS_MNK_POSITION_H
#define MCTS_MNK_POSITION_H

#include "constants.h"
#include "fixed_vector.h"
#include <vector>
#include <unordered_set>

struct Threats {
    std::unordered_set<Move> our_threats_1{};  // One move threats
    std::unordered_set<Move> our_threats_2{};  // Threats to create a chain with threats on both sides

    std::unordered_set<Move> opp_threats_1{};  // One move threats
    std::unordered_set<Move> opp_threats_2{};  // Threats to create a chain with threats on both sides
};

struct State {
    int last_piece = EMPTY;
    Move move{};
};

class Position {
public:

    bool is_empty(uint16_t row, uint16_t col);

    void get_moves(FixedVector<Move, MAX_MOVES>& moves);
    void get_direct_adjacent_moves(FixedVector<Move, MAX_MOVES>& moves);
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

    inline bool is_adjacent(uint16_t row, uint16_t col) {
        for (Increment increment : TRAVERSAL_INCREMENTS) {
            int new_row = row + increment.row;
            int new_col = col + increment.col;
            if (new_row < 0 || new_row >= BOARD_HEIGHT || new_col < 0 || new_col >= BOARD_WIDTH) continue;

            if (!is_empty(new_row, new_col)) return true;
        }

        return false;
    }

    template<bool adjacency>
    inline void make_move(Move move) {
        board[move.row][move.col] = side;
        side ^= 1;

        if constexpr (adjacency) {
            for (Increment increment : TRAVERSAL_INCREMENTS) {
                int new_row = move.row + increment.row;
                int new_col = move.col + increment.col;
                if (new_row < 0 || new_row >= BOARD_HEIGHT || new_col < 0 || new_col >= BOARD_WIDTH) continue;
                if (board[new_row][new_col] != EMPTY) continue;

                board[new_row][new_col] = ADJACENT;
            }
        }
    }

    template<bool adjacency>
    inline void undo_move(Move move) {
        board[move.row][move.col] = EMPTY;
        side ^= 1;

        if constexpr (adjacency) {

            /* Check if the current square should be adjacent or not and also
             * check if all adjacent squares are still adjacent
             */
            for (Increment increment : TRAVERSAL_INCREMENTS) {
                int new_row = move.row + increment.row;
                int new_col = move.col + increment.col;
                if (new_row < 0 || new_row >= BOARD_HEIGHT || new_col < 0 || new_col >= BOARD_WIDTH) continue;

                if (is_empty(new_row, new_col)) {
                    board[new_row][new_col] = is_adjacent(new_row, new_col) ? ADJACENT : EMPTY;
                } else {
                    board[move.row][move.col] = ADJACENT;
                }
            }

            // print_board();
        }
    }
};


#endif //MCTS_MNK_POSITION_H
