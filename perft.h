//
// Created by Alexander Tian on 11/17/23.
//

#ifndef MCTS_MNK_PERFT_H
#define MCTS_MNK_PERFT_H

#include "constants.h"
#include "position.h"

class PerftEngine {
    FixedVector<Move, MAX_MOVES> moves;
public:
    uint64_t perft(Position& position, PLY_TYPE depth);
};



#endif //MCTS_MNK_PERFT_H
