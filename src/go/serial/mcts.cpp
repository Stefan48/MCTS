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

int MCTS::simulateRandomPlayout(Node *node) {
    if (node->getBoard() == NULL) {
        node->constructBoard();
    }
    Board tempBoard = Board(*node->getBoard());
    while(tempBoard.isOngoing()) {
        tempBoard.applyRandomMove();
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

Position MCTS::getNextMove(Board *board, int numIterations) {
    Node *root = new Node(new Board(*board));
    /* could be limited by time */
    for (int i = 0; i < numIterations; ++i) {

        /*vector<Node*> children = root->getChildren();
        for (int i = 0; i < children.size(); ++i) {
            cout << i << ": " << children[i]->getUctValue() << "\n";
        }*/

        /* 1 - Selection */
        Node *promisingNode = selectPromisingNode(root);
        /* 2 - Expansion */
        expandNode(promisingNode);
        /* 3 - Simulation */
        Node *nodeToExplore = promisingNode;
        if (promisingNode->getChildren().size() > 0) {
            nodeToExplore = promisingNode->getRandomChildNode();
        }
        int result = simulateRandomPlayout(nodeToExplore);
        /* 4 - Backpropagation */
        backpropagate(nodeToExplore, result);
    }

    /*vector<Node*> children = root->getChildren();
    for (auto it = children.begin(); it != children.end(); ++it) {
        cout << (*it)->getWins() << "/" << (*it)->getVisits() << "\t";
    }
    cout << "\n";*/

    Position bestMove = root->getChildWithMaxVisits()->getLastMove();
    Node::freeNode(root);
    return bestMove;
}
