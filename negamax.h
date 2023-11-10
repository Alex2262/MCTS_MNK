//
// Created by Alexander Tian on 11/8/23.
//

#ifndef MCTS_MNK_NEGAMAX_H
#define MCTS_MNK_NEGAMAX_H

#include "constants.h"
#include "position.h"

class Engine {
public:
    Move best_move;

};

int negamax(Position position, int alpha, int beta, int depth);


#endif //MCTS_MNK_NEGAMAX_H
