#ifndef __NODE_H__
#define __NODE_H__

#include <bits/stdc++.h>
#include "position.h"
#include "board.h"
using namespace std;

class Node {
    Position lastMove;
    Board *board = NULL;
    int wins = 0;
    int visits = 0;
    Node *parent = NULL;
    vector<Node*> children;

public:
    Node();
    Node(Board *board);
    Node(Position lastMove, Node *parent);
    Node(Board *board, Node *parent, vector<Node*> children);

    static void freeNode(Node *node);

    Position getLastMove();
    Board *getBoard();
    void setBoard(Board *board);
    void constructBoard();
    Node *getParent();
    void setParent(Node *parent);
    vector<Node*> getChildren();
    void setChildren(vector<Node*> children);
    vector<Node*> getPossibleSuccessors();
    void addChild(Node *node);
    Node *getRandomChildNode();
    Node *getChildWithMaxVisits();
    int getWins();
    int getVisits();
    void incrementWins();
    void incrementVisits();
    double getUctValue();
    Node *getMaxUctChild();
};

#endif // __NODE_H__
