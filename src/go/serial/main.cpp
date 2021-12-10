#include <bits/stdc++.h>
#include "position.h"
#include "board.h"
#include "node.h"
#include "mcts.h"
using namespace std;

#define BOARD_SIZE 9
#define NUM_GAMES 10
#define ITERATIONS_PER_MOVE 100

int main() {
    srand(time(NULL));
    int winsBlack = 0, winsWhite = 0;
    
    for (int i = 0; i < NUM_GAMES; ++i) {
        Board board(BOARD_SIZE);
        while (board.isOngoing()) {
            board.applyMove(MCTS::getNextMove(&board, ITERATIONS_PER_MOVE));
        }
        if (board.getStatus(NULL, NULL) == Board::P1) {
            winsBlack++;
        }
        else {
            winsWhite++;
        }
        cout << "Game " << i << " finished\n";
    }
    cout << "Black " << winsBlack << " - " << winsWhite << " White\n";

    return 0;
}
