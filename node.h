#ifndef __NODE_H__
#define __NODE_H__

#include <bits/stdc++.h>
#include "state.h"
using namespace std;

class Node {
    State state;
    Node *parent = NULL;
    vector<Node*> children;

public:
    Node();
    Node(State state);
    Node(State state, Node *parent, vector<Node*> children);

    static void freeNode(Node *node);

    State getState();
    void setState(State state);
    Node *getParent();
    void setParent(Node *parent);
    vector<Node*> getChildren();
    void setChildren(vector<Node*> children);
    void addChild(Node *node);
    Node *getRandomChildNode();
    Node *getChildWithMaxVisits();
    void incrementWins();
    void incrementDraws();
    void incrementVisits();
    double getUctValue();
    Node *getMaxUctChild();
};

#endif // __NODE_H__
