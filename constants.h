//
// Created by Alexander Tian on 5/22/23.
//

#ifndef MCTS_MNK_CONSTANTS_H
#define MCTS_MNK_CONSTANTS_H

#include <cstdint>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

typedef uint16_t PLY_TYPE;

constexpr uint64_t MAX_ITERATIONS = 10000000;
constexpr uint64_t MAX_TIME = 5000;
constexpr double EXPLORATION_CONSTANT = 1.41;

constexpr int BOARD_HEIGHT = 15;
constexpr int BOARD_WIDTH = 15;
constexpr int WIN_AMT = 5;
constexpr int THREADS = 1;
constexpr int MAX_MOVES = BOARD_HEIGHT * BOARD_WIDTH;

constexpr PLY_TYPE MAX_SIMULATION_DEPTH = WIN_AMT * WIN_AMT + 9;
constexpr PLY_TYPE MAX_DEPTH = MAX_SIMULATION_DEPTH + 256;

constexpr int WHITE  = 0;
constexpr int BLACK  = 1;
constexpr int EMPTY  = 2;
constexpr int VISUAL = 3;
constexpr int ADJACENT = 4;

constexpr int DRAW_SCORE = 2;
constexpr int NO_SCORE = 3;

constexpr bool MOVE_ADJACENCY = true;
constexpr bool NO_MOVE_ADJACENCY = false;

const std::string RESET  = "\033[0m";
const std::string RED    = "\033[31m";
const std::string YELLOW = "\033[33m";
const std::string GREEN  = "\033[32m";
const std::string CYAN   = "\033[36m";

struct Move {
    uint16_t row = 0;
    uint16_t col = 0;
};

constexpr Move NO_MOVE = {BOARD_HEIGHT, BOARD_WIDTH};

namespace std {
    template <>
    struct hash<Move> {
        size_t operator()(const Move& move) const {
            return hash<int>()(move.row) ^ hash<int>()(move.col);
        }
    };
}

inline bool operator==(const Move& move1, const Move& move2) {
    return move1.row == move2.row && move1.col == move2.col;
}

struct Increment {
    int row = 0;
    int col = 0;
};

constexpr Increment get_opposite_increment(Increment increment) {
    return Increment{-increment.row, -increment.col};
}

constexpr Increment TRAVERSAL_INCREMENTS[8] = {
        Increment{-1, -1}, Increment{-1, 0}, Increment{-1, 1}, Increment{0, 1},
        Increment{1, 1}, Increment{1, 0}, Increment{1, -1}, Increment{0, -1}
};

constexpr Increment UNIQUE_INCREMENTS[4] = {
        Increment{-1, 0}, Increment{-1, 1}, Increment{0, 1}, Increment{1, 1},
};

template <typename Out>
inline void split(const std::string &s, char delim, Out result) {
    std::istringstream iss(s);
    std::string item;
    while (std::getline(iss, item, delim)) {
        *result++ = item;
    }
}

inline std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;
}

inline int get_chebyshev_distance(Move move_1, Move move_2) {
    if (move_1.row == BOARD_HEIGHT || move_2.row == BOARD_HEIGHT) return 0;

    return std::max(abs(static_cast<int>(move_1.row) - static_cast<int>(move_2.row)),
                    abs(static_cast<int>(move_1.col) - static_cast<int>(move_2.col)));
}


#endif //MCTS_MNK_CONSTANTS_H
