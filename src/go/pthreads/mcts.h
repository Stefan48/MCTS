#ifndef __MCTS_H__
#define __MCTS_H__

#include "node.h"

class MCTS {
    static Node *selectPromisingNode(Node *root);
    static void expandNode(Node *node);
    static int simulateRandomPlayout(Node *node, unsigned int *seed);
    static void backpropagate(Node *nodeToExplore, int result);

public:
    static void evaluateMoves(Board *board, vector< atomic<int> > *visits, int numIterations, unsigned int *seed);
};

#endif // __MCTS_H__
