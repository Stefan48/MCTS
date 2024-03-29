#include <bits/stdc++.h>
#include <mpi.h>
#include "position.h"
#include "board.h"
#include "node.h"
#include "mcts.h"
using namespace std;

#define BOARD_SIZE 9
#define NUM_GAMES 10
#define ITERATIONS_PER_MOVE 100

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int numProc, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    srand(time(NULL) + rank);
    int boardSize = BOARD_SIZE;
    int maxMoves = boardSize * boardSize + 1;
    int *visits = new int[maxMoves];
    int iterationsPerProcess = ITERATIONS_PER_MOVE / numProc + 1;

    if (rank == 0) {
        int winsBlack = 0, winsWhite = 0;
        int *visitsRecv = new int[maxMoves];
        for (int i = 0; i < NUM_GAMES; ++i) {
            Board board(boardSize);
            while (true) {
                MCTS::evaluateMoves(&board, visits, iterationsPerProcess);
                for(int p = 1; p < numProc; ++p) {
                    MPI_Recv(visitsRecv, maxMoves, MPI_INT, p, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    for (int j = 0; j < maxMoves; ++j) {
                        visits[j] += visitsRecv[j];
                    }
                }
                int chosenMove, maxVisits = -1;
                for (int j = 0; j < maxMoves; ++j) {
                    if (visits[j] > maxVisits) {
                        maxVisits = visits[j];
                        chosenMove = j;
                    }
                }
                MPI_Bcast(&chosenMove, 1, MPI_INT, 0, MPI_COMM_WORLD);
                if (chosenMove == 0) {
                    board.applyMove(Position(-1, -1));
                }
                else {
                    board.applyMove(Position((chosenMove-1)/boardSize, (chosenMove-1)%boardSize));
                }
                if (!board.isOngoing()) {
                    cout << "Game " << i << " finished\n";
                    if (board.getStatus(NULL, NULL) == Board::P1) {
                        winsBlack++;
                    }
                    else {
                        winsWhite++;
                    }
                    break;
                }
            } 
        }
        cout << "Black " << winsBlack << " - " << winsWhite << " White\n";
        delete[] visitsRecv;
    }
    else {
        int chosenMove;
        for (int i = 0; i < NUM_GAMES; ++i) {
            Board board(boardSize);
            while (true) {
                MCTS::evaluateMoves(&board, visits, iterationsPerProcess);
                MPI_Send(visits, maxMoves, MPI_INT, 0, 0, MPI_COMM_WORLD);
                MPI_Bcast(&chosenMove, 1, MPI_INT, 0, MPI_COMM_WORLD);
                if (chosenMove == 0) {
                    board.applyMove(Position(-1, -1));
                }
                else {
                    board.applyMove(Position((chosenMove-1)/boardSize, (chosenMove-1)%boardSize));
                }
                if (!board.isOngoing()) {
                    break;
                }
            }
        }
    }

    delete[] visits;
    MPI_Finalize();
    return 0;
}
