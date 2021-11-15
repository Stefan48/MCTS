#ifndef __BOARD_H__
#define __BOARD_H__

#include <bits/stdc++.h>
#include "position.h"
using namespace std;

class Board {
    public:
    int boardSize;
    int **board = NULL;
    int playerTurn = P1;
    int **prevBoard = NULL;
    int passes = 0;
    int winner = ONGOING;

    static bool equalBoards(int **b1, int **b2, int boardSize);
    void togglePlayerTurn();
    void updatePlayersFound(int &playersFound, int players);
    int countTerritory(Position pos, bool **visited, int &cnt);
    bool chainHasLiberties(int player, Position pos, bool **visited);
    bool isSuicideMove(Position pos);
    bool isKoMove(Position pos);
    void capturePieces(int playerCaptured, bool **visited);

public:
    constexpr static float KOMI = 6.5f;
    const static int ONGOING = 0;
    const static int P1 = 1;
    const static int P2 = 2;
    const static int DEFAULT_BOARD_SIZE = 19;
    const static int EMPTY = 0;

    static int getOpponent(int player);

    Board();
    Board(int boardSize);
    Board(int boardSize, int **board);
    Board(int boardSize, int **board, int playerTurn);
    Board(const Board &other);
    ~Board();
    Board& operator=(const Board &other);
    friend bool operator==(const Board &b1, const Board &b2);

    int **getBoard();
    void setBoard(int **board);
    int getBoardSize();
    int getPlayerTurn();
    int getCurrentOpponent();
    bool isOngoing();
    int getStatus(float *blackScore, float *whiteScore);
    void printBoard();
    void printStatus();
    vector<Position> getValidMoves();
    void applyMove(Position pos);
    void applyRandomMove(unsigned int *seed);
};

#endif // __BOARD_H__
