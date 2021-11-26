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
    int winsBlack = 0, winsWhite = 0;

    if (rank == 0) {
        int *visitsRecv = new int[maxMoves];
        for (int i = 0; i < NUM_GAMES; ++i) {
            Board board(boardSize);
            while (board.isOngoing()) {
                int playerTurn = board.getPlayerTurn();
                MPI_Bcast(&playerTurn, 1, MPI_INT, 0, MPI_COMM_WORLD);
                int passes = board.getPasses();
                MPI_Bcast(&passes, 1, MPI_INT, 0, MPI_COMM_WORLD);
                for(int i = 0; i < boardSize; ++i) {
                    MPI_Bcast(board.getBoard()[i], boardSize, MPI_INT, 0, MPI_COMM_WORLD);
                }
                MCTS::evaluateMoves(&board, visits, iterationsPerProcess);
                for(int j = 1; j < numProc; ++j) {
                    MPI_Recv(visitsRecv, maxMoves, MPI_INT, j, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    for (int k = 0; k < maxMoves; ++k) {
                        visits[k] += visitsRecv[k];
                    }
                }
                int bestMove, maxVisits = -1;
                for (int j = 0; j < maxMoves; ++j) {
                    if (visits[j] > maxVisits) {
                        maxVisits = visits[j];
                        bestMove = j;
                    }
                }
                Position pos;
                if (bestMove == 0) {
                    pos = Position(-1, -1);
                }
                else {
                    pos = Position((bestMove-1) / boardSize, (bestMove-1) % boardSize);
                }
                cout << pos.x << " " << pos.y << "\n";
                board.applyMove(pos);
                board.printBoard();
                cout << "\n";
                
                for(int i = 0; i < boardSize; ++i) {
                    MPI_Bcast(board.getPrevBoard()[i], boardSize, MPI_INT, 0, MPI_COMM_WORLD);
                }
            }
            cout << "Game " << i << " finished\n";
            if (board.getStatus(NULL, NULL) == Board::P1) {
                winsBlack++;
            }
            else {
                winsWhite++;
            }
        }
        /* stop condition */
        int stop = -1;
        MPI_Bcast(&stop, 1, MPI_INT, 0, MPI_COMM_WORLD);
        cout << "Black " << winsBlack << " - " << winsWhite << " White\n";
        delete[] visitsRecv;
    }
    else {
        Board board(boardSize);
        int **prevBoard = new int*[boardSize];
        for (int i = 0; i < boardSize; ++i) {
            prevBoard[i] = new int[boardSize];
        }
        while (true) {
            int playerTurn;
            MPI_Bcast(&playerTurn, 1, MPI_INT, 0, MPI_COMM_WORLD);
            if (playerTurn <= 0) {
                break;
            }
            board.setPlayerTurn(playerTurn);
            int passes;
            MPI_Bcast(&passes, 1, MPI_INT, 0, MPI_COMM_WORLD);
            board.setPasses(passes);
            for(int i = 0; i < boardSize; ++i) {
                MPI_Bcast(board.getBoard()[i], boardSize, MPI_INT, 0, MPI_COMM_WORLD);
            }
            MCTS::evaluateMoves(&board, visits, iterationsPerProcess);
            MPI_Send(visits, maxMoves, MPI_INT, 0, 0, MPI_COMM_WORLD);
            for(int i = 0; i < boardSize; ++i) {
                MPI_Bcast(prevBoard[i], boardSize, MPI_INT, 0, MPI_COMM_WORLD);
            }
            board.setPrevBoard(prevBoard);
        }
    }

    delete[] visits;
    MPI_Finalize();
    return 0;
}
