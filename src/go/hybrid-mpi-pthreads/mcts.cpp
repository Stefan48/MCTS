#include "mcts.h"

Node* MCTS::selectPromisingNode(Node *root) {
    Node *node = root;
    while (node->getChildren().size()) {
        node = node->getMaxUctChild();
    }
    return node;
}

void MCTS::expandNode(Node *node) {
    if (node->getBoard() == NULL) {
        node->constructBoard();
    }
    if (node->getBoard()->isOngoing()) {
        node->setChildren(node->getPossibleSuccessors());
    }
}

int MCTS::simulateRandomPlayout(Node *node, unsigned int *seed) {
    if (node->getBoard() == NULL) {
        node->constructBoard();
    }
    Board tempBoard = Board(*node->getBoard());
    while(tempBoard.isOngoing()) {
        tempBoard.applyRandomMove(seed);
    }
    return tempBoard.getStatus(NULL, NULL);
}

void MCTS::backpropagate(Node *nodeToExplore, int result) {
    Node *node = nodeToExplore;
    while (node) {
        node->incrementVisits();
        if (result == Board::P1) {
            node->incrementWins();
        }
        node = node->getParent();
    }
}

void MCTS::evaluateMoves(Board *board, vector< atomic <int> > *visits, int numIterations, unsigned int *seed) {
    Node *root = new Node(new Board(*board));
    /* could be limited by time */
    for (int i = 0; i < numIterations; ++i) {
        /* 1 - Selection */
        Node *promisingNode = selectPromisingNode(root);
        /* 2 - Expansion */
        expandNode(promisingNode);
        /* 3 - Simulation */
        Node *nodeToExplore = promisingNode;
        if (promisingNode->getChildren().size() > 0) {
            nodeToExplore = promisingNode->getRandomChildNode(seed);
        }
        int result = simulateRandomPlayout(nodeToExplore, seed);
        /* 4 - Backpropagation */
        backpropagate(nodeToExplore, result);
    }
    vector<Node*> children = root->getChildren();
    for (int i = 0; i < (int)children.size(); ++i) {
        Position pos = children[i]->getLastMove();
        if (pos.x == -1 && pos.y == -1) {
            (*visits)[0] += children[i]->getVisits();
        }
        else {
            (*visits)[pos.x * board->getBoardSize() + pos.y + 1] += children[i]->getVisits();
        }
    }
    Node::freeNode(root);
}
