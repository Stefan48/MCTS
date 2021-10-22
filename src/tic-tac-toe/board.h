#ifndef __BOARD_H__
#define __BOARD_H__

#include <bits/stdc++.h>
#include "position.h"
using namespace std;

class Board {
    int boardSize;
    int **board = NULL;

public:
    const static int ONGOING = 0;
    const static int P1 = 1;
    const static int P2 = 2;
    const static int DRAW = 3;
    const static int DEFAULT_BOARD_SIZE = 3;
    const static int EMPTY = 0;

    static int getOpponent(int player);

    Board();
    Board(int boardSize);
    Board(int boardSize, int **board);
    Board(const Board &other);
    ~Board();
    Board& operator=(const Board &other);

    int **getBoard();
    void setBoard(int **board);
    int getBoardSize();
    int getStatus();
    void printBoard();
    void printStatus();
    vector<Position> getValidMoves();
    void applyMove(int player, Position pos);
};

#endif // __BOARD_H__
