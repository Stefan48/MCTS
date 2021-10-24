#ifndef __BOARD_H__
#define __BOARD_H__

#include <bits/stdc++.h>
#include "position.h"
using namespace std;

class Board {
    int boardSize;
    int **board = NULL;
    Board *prevBoard = NULL;

public:
    const static int ONGOING = 0;
    const static int P1 = 1;
    const static int P2 = 2;
    const static int DEFAULT_BOARD_SIZE = 19;
    const static int EMPTY = 0;

    static int getOpponent(int player);

    Board();
    Board(int boardSize);
    Board(int boardSize, int **board);
    Board(const Board &other);
    ~Board();
    Board& operator=(const Board &other);
    friend bool operator==(const Board &b1, const Board &b2);

    int **getBoard();
    void setBoard(int **board);
    int getBoardSize();
    int getStatus();
    void printBoard();
    void printStatus();
    vector<Position> getValidMoves(int player);
    void applyMove(int player, Position pos);
    bool isSuicideMove(int player, Position pos);
    bool isKoMove(int playerTurn, Position pos);
    bool chainHasLiberties(int player, Position pos, bool** visited);
    void capturePieces(int opponent, Position pos,bool** visited);
};

#endif // __BOARD_H__
