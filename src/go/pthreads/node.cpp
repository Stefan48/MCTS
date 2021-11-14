#include "node.h"

Node::Node() {}

Node::Node(Board *board) {
    this->board = board;
}

Node::Node(Position lastMove, Node *parent) {
    this->lastMove = lastMove;
    this->parent = parent;
}

Node::Node(Board *board, Node *parent, vector<Node*> children) {
    this->board = board;
    this->parent = parent;
    this->children = children;
}

void Node::freeNode(Node *node) {
    for (auto it = node->children.begin(); it != node->children.end(); ++it) {
        freeNode(*it);
    }
    if(node->board) {
        delete node->board;
    }
    delete node;
}

Position Node::getLastMove() {
    return lastMove;
}

Board* Node::getBoard() {
    return board;
}

void Node::setBoard(Board *board) {
    this->board = board;
}

void Node::constructBoard() {
    /* should be called only if the node has a parent and a lastMove but no board */
    Board *parentBoard = parent->board;
    Board *board = new Board(*parentBoard);
    board->applyMove(lastMove);
    this->board = board;
}

Node* Node::getParent() {
    return parent;
}

void Node::setParent(Node *parent) {
    this->parent = parent;
}

vector<Node*> Node::getChildren() {
    return children;
}

void Node::setChildren(vector<Node*> children) {
    this->children = children;
}

vector<Node*> Node::getPossibleSuccessors() {
    /* assume the node has a board */
    vector<Node*> possibleSuccessors;
    vector<Position> moves = board->getValidMoves();
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        Node *newNode = new Node(*it, this);
        possibleSuccessors.push_back(newNode);
    }
    return possibleSuccessors;
}

void Node::addChild(Node *node) {
    children.push_back(node);
}

Node* Node::getRandomChildNode() {
    // TODO replace 'rand'
    return children[rand() % children.size()];
}

Node* Node::getChildWithMaxVisits() {
    Node *child;
    int maxVisits = INT_MIN, visits;
    for (int i = 0; i < (int)children.size(); ++i) {
        visits = children[i]->visits;
        /* greater or equal in order to avoid passes */
        if (visits >= maxVisits) {
            maxVisits = visits;
            child = children[i];
        }
    }
    return child;
}

int Node::getWins() {
    return wins;
}

int Node::getVisits() {
    return visits;
}

void Node::incrementWins() {
    this->wins++;
}

void Node::incrementVisits() {
    this->visits++;
}

double Node::getUctValue() {
    int perspective = this->parent->getBoard()->getPlayerTurn();
    int parentVisits = this->parent->visits;

    if (visits == 0) {
        return std::numeric_limits<double>::max();
    }
    if (perspective == Board::P1) {
        return wins / (double)visits + sqrt(2*log(parentVisits) / (double)visits);
    }
    /* complement the exploitation factor (and keep the exploration factor) */
    return 1 - wins / (double)visits + sqrt(2*log(parentVisits) / (double)visits);
}

Node* Node::getMaxUctChild() {
    vector<Node*> children = this->getChildren();
    Node *maxUctNode = NULL;
    double maxUctValue = -std::numeric_limits<double>::max();
    double uctValue;
    for (int i = 0; i < (int)children.size(); ++i) {
        uctValue = children[i]->getUctValue();
        if (uctValue > maxUctValue) {
            maxUctValue = uctValue;
            maxUctNode = children[i];
        }
    }
    return maxUctNode;
}
