#include <bits/stdc++.h>
#include "position.h"
#include "board.h"
#include "state.h"
#include "node.h"
#include "mcts.h"
using namespace std;

int main()
{
    /* srand(time(NULL));
    Board board;
    int playerTurn = Board::P1;
    board.printBoard();
    cout << '\n';
    for(;;) {
        board = MCTS::getNextMove(board, playerTurn);
        playerTurn = Board::getOpponent(playerTurn);
        board.printBoard();
        cout << '\n';
        if (board.getStatus() != Board::ONGOING) {
            break;
        }
    }
    board.printStatus(); */

    srand(time(NULL));
    int draws = 0, winsP1 = 0, winsP2 = 0;
    for (int i = 0; i < 1; ++i) {
        Board board(13);
        int playerTurn = Board::P1;
        while (board.getStatus() == Board::ONGOING) {
            board.printBoard();
            cout << "\n";
            board = MCTS::getNextMove(board, playerTurn);
            cout << "!\n";
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
    cout << "draws: " << draws << "\nwins X: " << winsP1 << "\nwins 0: " << winsP2 << "\n";

    /* srand(time(NULL));
    int **v = new int*[3];
    for (int i = 0; i < 3; ++i) {
        v[i] = new int[3]();
    }
    v[0][0] = 0; v[0][1] = 0; v[0][2] = 0;
    v[1][0] = 0; v[1][1] = 1; v[1][2] = 0;
    v[2][0] = 0; v[2][1] = 0; v[2][2] = 0;
    Board board(3, v);
    // cout << UCT::getUctValue(1, 1, 1, 1);

    State state(board, 2);
    // Node *root = new Node(state);
    // Node *n = MCTS::selectPromisingNode(root);
    // MCTS::expandNode(root);
    // vector<Node*> children = root->getChildren();
    // cout << children.size() << "\n";
    // for (auto it = children.begin(); it != children.end(); ++it) {
    //    int result = MCTS::simulateRandomPlayout(*it);
    //    cout << result << "\n";
    //    MCTS::backpropagate(*it, result);
    //    cout << "----------\n";
    // }
    MCTS::getNextMove(board, 2).printBoard(); */

    /* int **v = new int*[5];
    for (int i = 0; i < 5; ++i) {
        v[i] = new int[5]();
    }
    v[0][0] = 0; v[0][1] = 1; v[0][2] = 2; v[0][3] = 0; v[0][4] = 0;
    v[1][0] = 1; v[1][1] = 0; v[1][2] = 1; v[1][3] = 2; v[1][4] = 0;
    v[2][0] = 0; v[2][1] = 1; v[2][2] = 2; v[2][3] = 0; v[2][4] = 0;
    v[3][0] = 0; v[3][1] = 0; v[3][2] = 0; v[3][3] = 0; v[3][4] = 0;
    v[4][0] = 0; v[4][1] = 0; v[4][2] = 0; v[4][3] = 0; v[4][4] = 0;
    Board board(5, v);
    //cout << (board.isSuicideMove(2, Position(1,1)) ? "is suicide\n" : "isn't suicide\n");
    //ard.applyMove(2, Position(1,1));
    //board.printBoard();
    //cout<<board.isKoMove(1,Position(1,2));
    vector<Position> moves = board.getValidMoves(2);
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        cout << it->x << " " << it->y << "\n";
    }
    cout << "\n"; */
    return 0;
}
