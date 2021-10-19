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
        Board board;
        int playerTurn = Board::P1;
        while (board.getStatus() == Board::ONGOING) {
            board = MCTS::getNextMove(board, playerTurn);
            playerTurn = Board::getOpponent(playerTurn);
        }
        int status = board.getStatus();
        if (status == Board::DRAW) {
            draws++;
        }
        else if (status == Board::P1) {
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


    return 0;
}
