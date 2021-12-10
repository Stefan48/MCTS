#include <bits/stdc++.h>
#include <mpi.h>
#include <omp.h>
#include <sys/sysinfo.h>
#include "position.h"
#include "board.h"
#include "node.h"
#include "mcts.h"
using namespace std;

#define MAX_THREADS 1024
#define BOARD_SIZE 9
#define NUM_GAMES 1
#define ITERATIONS_PER_MOVE 5000

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int numProc, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Board board;
    bool ongoing;
    int maxMoves = BOARD_SIZE * BOARD_SIZE + 1;
    vector< atomic<int> > visits;
    int numThreads = get_nprocs();
    cout << "rank=" << rank << " numThreads=" << numThreads << "\n";
    omp_set_num_threads(numThreads);
    int iterationsPerThread = ITERATIONS_PER_MOVE / (numProc * numThreads) + 1;
    
    if (rank == 0) {
        #pragma omp parallel default(none) shared(numProc, board, ongoing, visits, iterationsPerThread, maxMoves, ompi_mpi_comm_world, ompi_mpi_int, ompi_mpi_cxx_bool, cout)
        {
            int threadId = omp_get_thread_num();
            unsigned int seed = time(NULL) + threadId;
            if (threadId == 0) {
                int winsBlack = 0, winsWhite = 0;
                int *visitsRecv = new int[maxMoves];
                board = Board(BOARD_SIZE);
                ongoing = true;
                visits = vector< atomic<int> >(maxMoves);
                #pragma omp barrier
                for (int i = 0; i < NUM_GAMES; ++i) {
                    while (true) {
                        MCTS::evaluateMoves(&board, &visits, iterationsPerThread, &seed);
                        #pragma omp barrier
                        for (int p = 1; p < numProc; ++p) {
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
                            board.applyMove(Position((chosenMove-1)/board.getBoardSize(), (chosenMove-1)%board.getBoardSize()));
                        }
                        //board.printBoard();
                        //cout << "\n";
                        ongoing = board.isOngoing();
                        if (ongoing) {
                            visits = vector< atomic<int> >(maxMoves);
                            #pragma omp barrier
                        }
                        else {
                            cout << "Game " << i << " finished\n";
                            if (board.getStatus(NULL, NULL) == Board::P1) {
                                winsBlack++;
                            }
                            else {
                                winsWhite++;
                            }
                            if (i < NUM_GAMES - 1) {
                                bool stop = false;
                                MPI_Bcast(&stop, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
                                board = Board(BOARD_SIZE);
                                ongoing = true;
                                visits = vector< atomic<int> >(maxMoves);
                            }
                            else {
                                bool stop = true;
                                MPI_Bcast(&stop, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
                            }
                            #pragma omp barrier
                            break;
                        }
                    }
                }
                cout << "Black " << winsBlack << " - " << winsWhite << " White\n";
                   delete[] visitsRecv;
            }
            else {
                #pragma omp barrier
                while (ongoing) {
                    MCTS::evaluateMoves(&board, &visits, iterationsPerThread, &seed);
                    #pragma omp barrier
                    /* wait for master thread to find out and apply the chosen move */
                    #pragma omp barrier
                }
            }
        }
    }
    else {
        #pragma omp parallel default(none) shared(numProc, rank, board, ongoing, visits, iterationsPerThread, maxMoves, ompi_mpi_comm_world, ompi_mpi_int, ompi_mpi_cxx_bool, cout)
        {
            int threadId = omp_get_thread_num();
            unsigned int seed = time(NULL) + rank * MAX_THREADS + threadId;
            if (threadId == 0) {
                int *visitsSend;
                bool differentSizes = (sizeof(atomic<int>) != sizeof(int));
                if (differentSizes) {
                    visitsSend = new int[maxMoves];
                }
                board = Board(BOARD_SIZE);
                ongoing = true;
                visits = vector< atomic<int> >(maxMoves);
                int chosenMove;
                bool stop;
                #pragma omp barrier
                for (int i = 0; i < NUM_GAMES; ++i) {
                    while (true) {
                        MCTS::evaluateMoves(&board, &visits, iterationsPerThread, &seed);
                        #pragma omp barrier
                        if (differentSizes) {
                            for (int j = 0; j < maxMoves; ++j) {
                                visitsSend[j] = visits[j];
                            }
                            MPI_Send(visitsSend, maxMoves, MPI_INT, 0, 0, MPI_COMM_WORLD);
                        }
                        else {
                            MPI_Send(&visits[0], maxMoves, MPI_INT, 0, 0, MPI_COMM_WORLD);
                        }
                        MPI_Bcast(&chosenMove, 1, MPI_INT, 0, MPI_COMM_WORLD);
                        if (chosenMove == 0) {
                            board.applyMove(Position(-1, -1));
                        }
                        else {
                            board.applyMove(Position((chosenMove-1)/board.getBoardSize(), (chosenMove-1)%board.getBoardSize()));
                        }
                        ongoing = board.isOngoing();
                        if (ongoing) {
                            visits = vector< atomic<int> >(maxMoves);
                            #pragma omp barrier
                        }
                        else {
                            MPI_Bcast(&stop, 1, MPI_CXX_BOOL, 0, MPI_COMM_WORLD);
                            if (!stop) {
                                board = Board(BOARD_SIZE);
                                ongoing = true;
                                visits = vector< atomic<int> >(maxMoves);
                            }
                            #pragma omp barrier
                            break;
                        }
                    }
                }
                if (differentSizes) {
                    delete[] visitsSend;
                }
            }
            else {
                #pragma omp barrier
                while (ongoing) {
                    MCTS::evaluateMoves(&board, &visits, iterationsPerThread, &seed);
                    #pragma omp barrier
                    /* wait for master thread to find out and apply the chosen move */
                    #pragma omp barrier
                }
            }
        }
    }
    
    MPI_Finalize();
    return 0;
}
