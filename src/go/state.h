#ifndef __STATE_H__
#define __STATE_H__

#include "board.h"

class State {
    Board board;
    int playerTurn = Board::P1;
    int wins = 0;
    int visits = 0;

public:
    State();
    State(Board board);
    State(Board board, int playerTurn);

    Board getBoard();
    void setBoard(Board board);
    int getPlayerTurn();
    void setPlayerTurn(int playerTurn);
    int getOpponent();
    void togglePlayerTurn();
    int getWins();
    void incrementWins();
    int getVisits();
    void incrementVisits();
    vector<State> getPossibleStates();
    void applyRandomMove();
};

#endif // __STATE_H__
