#include "mcts.h"

Node* MCTS::selectPromisingNode(Node *root) {
    Node *node = root;
    while (node->getChildren().size()) {
        node = node->getMaxUctChild();
    }
    return node;
}

void MCTS::expandNode(Node *node) {
    vector<State> possibleStates = node->getState().getPossibleStates();
    for (auto it = possibleStates.begin(); it != possibleStates.end(); ++it) {
        Node *newNode = new Node(*it);
        newNode->setParent(node);
        node->addChild(newNode);
    }
}

int MCTS::simulateRandomPlayout(Node *node) {
    State tempState = node->getState();
    int boardStatus = tempState.getBoard().getStatus();

    while (boardStatus == Board::ONGOING) {
        tempState.applyRandomMove();
        boardStatus = tempState.getBoard().getStatus();
    }
    return boardStatus;
}

void MCTS::backpropagate(Node *nodeToExplore, int result) {
    Node *node = nodeToExplore;
    while (node != NULL) {
        node->incrementVisits();
        if (result == Board::P1) {
            node->incrementWins();
        }
        else if (result == Board::DRAW) {
            node->incrementDraws();
        }
        node = node->getParent();
    }
}

Board MCTS::getNextMove(Board board, int playerTurn) {
    Node *root = new Node(State(board, playerTurn));
    // could be limited by time
    for (int i = 0; i < 5000; ++i) {

        /* vector<Node*> children = root->getChildren();
        for (int i = 0; i < children.size(); ++i) {
            cout << i << ": " << children[i]->getUctValue() << "\n";
        } */

        /* 1 - Selection */
        Node *promisingNode = selectPromisingNode(root);
        /* 2 - Expansion */
        if (promisingNode->getState().getBoard().getStatus() == Board::ONGOING) {
            expandNode(promisingNode);
        }
        /* 3 - Simulation */
        Node *nodeToExplore = promisingNode;
        if (promisingNode->getChildren().size() > 0) {
            nodeToExplore = promisingNode->getRandomChildNode();
        }
        int result = simulateRandomPlayout(nodeToExplore);
        /* 4 - Backpropagation */
        backpropagate(nodeToExplore, result);
    }

    /* vector<Node*> children = root->getChildren();
    for (auto it = children.begin(); it != children.end(); ++it) {
        cout << (*it)->getState().getWins() << "/" << (*it)->getState().getVisits() << "\n";
    }
    cout << "\n"; */

    Node *bestNode = root->getChildWithMaxVisits();
    Board newBoard = bestNode->getState().getBoard();
    Node::freeNode(root);
    return newBoard;
}
