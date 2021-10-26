#include "node.h"

Node::Node() {}

Node::Node(State state) {
    this->state = state;
}

Node::Node(State state, Node *parent, vector<Node*> children) {
    this->state = state;
    this->parent = parent;
    this->children = children;
}

void Node::freeNode(Node *node) {
    for (auto it = node->children.begin(); it != node->children.end(); ++it) {
        freeNode(*it);
    }
    delete node;
}

State Node::getState() {
    return state;
}

void Node::setState(State state) {
    this->state = state;
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

void Node::addChild(Node *node) {
    children.push_back(node);
}

Node* Node::getRandomChildNode() {
    return children[rand() % children.size()];
}

Node* Node::getChildWithMaxVisits() {
    Node *child;
    int maxVisits = INT_MIN, visits;
    for (int i = 0; i < (int)children.size(); ++i) {
        visits = children[i]->getState().getVisits();
        if (visits > maxVisits) {
            maxVisits = visits;
            child = children[i];
        }
    }
    return child;
}

void Node::incrementWins() {
    this->state.incrementWins();
}

void Node::incrementVisits() {
    this->state.incrementVisits();
}

double Node::getUctValue() {
    int perspective = this->getState().getOpponent();
    int wins = this->getState().getWins();
    int nodeVisits = this->getState().getVisits();
    int parentVisits = this->parent->getState().getVisits();

    if (nodeVisits == 0) {
        return std::numeric_limits<double>::max();
    }
    if (perspective == Board::P1) {
        return wins / (double)nodeVisits + sqrt(2*log(parentVisits) / (double)nodeVisits);
    }
    /* complement the exploitation factor (and keep the exploration factor) */
    return 1 - wins / (double)nodeVisits + sqrt(2*log(parentVisits) / (double)nodeVisits);
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
