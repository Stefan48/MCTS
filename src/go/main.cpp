#include <bits/stdc++.h>
#include "position.h"
#include "board.h"
#include "node.h"
#include "mcts.h"
using namespace std;

int main()
{
    srand(time(NULL));
    Board board(9);
    Position pos;
    board.printBoard();
    cout << '\n';
    while (board.isOngoing()) {
        pos = MCTS::getNextMove(&board);
        cout << pos.x << ' ' << pos.y << '\n';
        board.applyMove(pos);
        board.printBoard();
        cout << '\n';
    }
    board.printStatus();


    /* srand(time(NULL));
    int draws = 0, winsP1 = 0, winsP2 = 0;
    for (int i = 0; i < 10; ++i) {
        Board board(13);
        int playerTurn = Board::P1;
        while (board.getStatus() == Board::ONGOING) {
            board = MCTS::getNextMove(board, playerTurn);
            playerTurn = Board::getOpponent(playerTurn);
        }
        int status = board.getStatus();
        if (status == Board::P1) {
            winsP1++;
        }
        else {
            winsP2++;
        }
    }
    cout << "draws: " << draws << "\nwins X: " << winsP1 << "\nwins 0: " << winsP2 << "\n"; */



    /* srand(time(NULL));
    int **v = new int*[5];
    for (int i = 0; i < 5; ++i) {
        v[i] = new int[5]();
    }
    v[0][0] = 0; v[0][1] = 1; v[0][2] = 0; v[0][3] = 0; v[0][4] = 1;
    v[1][0] = 1; v[1][1] = 0; v[1][2] = 1; v[1][3] = 1; v[1][4] = 0;
    v[2][0] = 0; v[2][1] = 1; v[2][2] = 2; v[2][3] = 2; v[2][4] = 1;
    v[3][0] = 1; v[3][1] = 2; v[3][2] = 0; v[3][3] = 2; v[3][4] = 2;
    v[4][0] = 2; v[4][1] = 0; v[4][2] = 2; v[4][3] = 0; v[4][4] = 0;
    Board board(5, v, Board::P1);
    board.printBoard();
    cout << '\n';
    vector<Position> moves = board.getValidMoves();
    for (unsigned int i = 0; i < moves.size(); ++i) {
        cout << moves[i].x << " " << moves[i].y << "\n";
    } */

    return 0;
}
