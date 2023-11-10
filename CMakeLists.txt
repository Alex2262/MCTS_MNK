cmake_minimum_required(VERSION 3.21)
project(MCTS_MNK)

set(CMAKE_CXX_STANDARD 20)

add_executable(MCTS_MNK constants.h position.cpp position.h mcts.cpp mcts.h main.cpp negamax.cpp negamax.h)
