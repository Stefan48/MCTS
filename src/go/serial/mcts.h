#ifndef __MCTS_H__
#define __MCTS_H__

#include "node.h"

class MCTS {
    static Node *selectPromisingNode(Node *root);
    static void expandNode(Node *node);
    static int simulateRandomPlayout(Node *node);
    static void backpropagate(Node *nodeToExplore, int result);

public:
    static Position getNextMove(Board *board, int numIterations);
};

#endif // __MCTS_H__
