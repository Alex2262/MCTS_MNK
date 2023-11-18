//
// Created by Alexander Tian on 5/22/23.
//

#include <iostream>
#include "position.h"


bool Position::is_empty(uint16_t row, uint16_t col) {
    return board[row][col] == EMPTY || board[row][col] == ADJACENT;
}

void Position::get_moves(FixedVector<Move, MAX_MOVES>& moves) {
    moves.clear();
    for (uint16_t row = 0; row < BOARD_HEIGHT; row++) {
        for (uint16_t col = 0; col < BOARD_WIDTH; col++) {
            if (is_empty(row, col)) {
                moves.push_back(Move{row=row, col=col});
            }
        }
    }
}

void Position::get_direct_adjacent_moves(FixedVector<Move, MAX_MOVES>& moves) {
    moves.clear();
    for (uint16_t row = 0; row < BOARD_HEIGHT; row++) {
        for (uint16_t col = 0; col < BOARD_WIDTH; col++) {
            if (board[row][col] == ADJACENT) {
                moves.push_back(Move{row=row, col=col});
            }
        }
    }
}

std::vector<Move> Position::get_adjacent_moves(int adjacency_range) {
    std::vector<Move> moves;

    for (uint16_t row = 0; row < BOARD_HEIGHT; row++) {
        for (uint16_t col = 0; col < BOARD_WIDTH; col++) {
            if (is_empty(row, col)) {
                bool is_adjacent = false;
                for (int r = -adjacency_range; r <= adjacency_range; r++) {
                    for (int c = -adjacency_range; c <= adjacency_range; c++) {
                        int new_row = r + row;
                        int new_col = c + col;
                        if (new_row < 0 || new_row >= BOARD_HEIGHT || new_col < 0 || new_col >= BOARD_WIDTH) continue;

                        // if (new_row == row && new_col == col) continue; NO NEED TO CHECK

                        if (!is_empty(new_row, new_col)) {
                            is_adjacent = true;
                            break;
                        }
                    }
                    if (is_adjacent) break;
                }

                if (is_adjacent) moves.push_back(Move{row=row, col=col});
            }
        }
    }

    return moves;
}

void Position::ray_threats(Threats& threats, int color, uint16_t row, uint16_t col, Increment increment) {
    Increment opposite_increment = get_opposite_increment(increment);
    std::unordered_set<Move>& threats_1 = color == side ? threats.our_threats_1 : threats.opp_threats_1;
    std::unordered_set<Move>& threats_2 = color == side ? threats.our_threats_2 : threats.opp_threats_2;

    int count = 0;
    int opposite_count = 0;
    bool empty_end = false;
    bool opposite_empty_end = false;


    // FORWARD RAY
    int new_row = row;
    int new_col = col;
    for (int i = 0; i < WIN_AMT - 1; i++) {
        new_row += increment.row;
        new_col += increment.col;

        if (new_row < 0 || new_row >= BOARD_HEIGHT || new_col < 0 || new_col >= BOARD_WIDTH) break;

        if (board[new_row][new_col] == !color) break;
        if (is_empty(new_row, new_col)) { empty_end = true; break; }
        count++;
    }

    // OPPOSITE RAY
    new_row = row;
    new_col = col;
    for (int i = 0; i < WIN_AMT - 1; i++) {
        new_row += opposite_increment.row;
        new_col += opposite_increment.col;

        if (new_row < 0 || new_row >= BOARD_HEIGHT || new_col < 0 || new_col >= BOARD_WIDTH) break;

        if (board[new_row][new_col] == !color) break;
        if (is_empty(new_row, new_col)) { opposite_empty_end = true; break; }
        opposite_count++;
    }

    if (count + opposite_count >= WIN_AMT - 1) threats_1.insert(Move{row, col});
    else if (count + opposite_count == WIN_AMT - 2 && empty_end && opposite_empty_end) threats_2.insert(Move{row, col});
}

void Position::get_square_threats(Threats& threats, uint16_t row, uint16_t col) {
    for (Increment increment : UNIQUE_INCREMENTS) {
        ray_threats(threats, WHITE, row, col, increment);
        ray_threats(threats, BLACK, row, col, increment);
    }
}

void Position::get_threats(Threats& threats) {
    for (uint16_t row = 0; row < BOARD_HEIGHT; row++) {
        for (uint16_t col = 0; col < BOARD_WIDTH; col++) {
            if (board[row][col] == ADJACENT) {
                get_square_threats(threats, row, col);
            }
        }
    }
}

int Position::get_result(Move last_move) {
    if (last_move.row == BOARD_HEIGHT) {
        return NO_SCORE;
    }

    int color = board[last_move.row][last_move.col];
    for (int test_y = last_move.row - (WIN_AMT - 1) / 2; test_y < last_move.row + (WIN_AMT + 1) / 2; test_y++) {
        for (int test_x = last_move.col - (WIN_AMT - 1) / 2; test_x < last_move.col + (WIN_AMT + 1) / 2; test_x++) {
            if (test_y < 0 || test_y >= BOARD_HEIGHT || test_x < 0 || test_x >= BOARD_WIDTH) continue;
            if (board[test_y][test_x] != color) continue;

            for (Increment increment : TRAVERSAL_INCREMENTS) {
                int new_y = test_y;
                int new_x = test_x;
                for (int i = 1; i < WIN_AMT; i++) {
                    new_y += increment.row;
                    new_x += increment.col;

                    if (new_y < 0 || new_y >= BOARD_HEIGHT || new_x < 0 || new_x >= BOARD_WIDTH) break;
                    if (board[new_y][new_x] != color) break;

                    if (i + 1 == WIN_AMT) return color;
                }
            }
        }
    }

    return NO_SCORE;
}


void Position::print_board() {
    std::string string = "  ";
    int max_digits = static_cast<int>(std::max(std::to_string(BOARD_HEIGHT).length(), std::to_string(BOARD_WIDTH).length()));

    for (int col = 0; col < BOARD_WIDTH; col++) {
        std::string col_str = std::to_string(col);
        for (int i = 0; i < max_digits - col_str.length(); i++) string += "0";
        string += col_str;
        string += " ";
    }

    string += "\n";

    for (int row = 0; row < BOARD_HEIGHT; row++) {
        std::string row_str = std::to_string(row);
        for (int i = 0; i < max_digits - row_str.length(); i++) string += "0";
        string += std::to_string(row);
        string += " ";

        for (int col = 0; col < BOARD_WIDTH; col++) {
            if (board[row][col] == WHITE) {
                string += "O";
            }
            else if (board[row][col] == BLACK) {
                string += RED + "X" + RESET;
            }
            else string += ".";

            for (int j = 0; j < max_digits; j++) string += " ";
        }
        string += "\n";
    }

    std::cout << string;
}


void Position::visualize_moves(const std::vector<Move>& moves) {

    if (moves.empty()) return;

    std::string string = "  ";
    int max_digits = static_cast<int>(std::max(std::to_string(BOARD_HEIGHT).length(), std::to_string(BOARD_WIDTH).length()));

    std::vector<std::pair<Move, int>> replacement{};

    for (Move move : moves) {
        replacement.emplace_back(move, board[move.row][move.col]);
        board[move.row][move.col] = VISUAL;
    }

    for (int col = 0; col < BOARD_WIDTH; col++) {
        std::string col_str = std::to_string(col);
        for (int i = 0; i < max_digits - col_str.length(); i++) string += "0";
        string += col_str;
        string += " ";
    }

    string += "\n";

    for (int row = 0; row < BOARD_HEIGHT; row++) {
        std::string row_str = std::to_string(row);
        for (int i = 0; i < max_digits - row_str.length(); i++) string += "0";
        string += std::to_string(row);
        string += " ";

        for (int col = 0; col < BOARD_WIDTH; col++) {
            if (board[row][col] == WHITE) {
                string += "O";
            }
            else if (board[row][col] == BLACK) {
                string += RED + "X" + RESET;
            }
            else if (board[row][col] == VISUAL) {
                string += GREEN + "*" + RESET;
            }
            else {
                string += ".";
            }

            for (int j = 0; j < max_digits; j++) string += " ";
        }
        string += "\n";
    }

    for (std::pair<Move, int> replace : replacement) {
        Move move = replace.first;
        int replace_value = replace.second;
        board[move.row][move.col] = replace_value;
    }

    std::cout << string;
}