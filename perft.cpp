//
// Created by Alexander Tian on 11/17/23.
//

#include <iostream>
#include "perft.h"

uint64_t PerftEngine::perft(Position& position, PLY_TYPE depth) {
    uint64_t nodes = 0;

    position.get_moves(moves);
    if (depth == 1) return moves.size();

    for (Move move : moves) {
        position.make_move<NO_MOVE_ADJACENCY>(move);
        nodes += perft(position, depth - 1);
        position.undo_move<NO_MOVE_ADJACENCY>(move);
    }

    return nodes;
}