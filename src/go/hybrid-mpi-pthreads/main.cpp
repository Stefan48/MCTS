#include <bits/stdc++.h>
#include <mpi.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include "position.h"
#include "board.h"
#include "node.h"
#include "mcts.h"
using namespace std;

#define MAX_THREADS 1024
#define BOARD_SIZE 9
#define NUM_GAMES 10
#define ITERATIONS_PER_MOVE 100

struct ThreadArgs {
    int threadId;
    Board *board;
    bool *ongoing;
    vector< atomic<int> > *visits;
    int numIterations;
    pthread_barrier_t *barrier;
};

void *threadFunction(void *arg)
{
    ThreadArgs *args = (ThreadArgs*)arg;
    unsigned int seed = time(NULL) + args->threadId;
    pthread_barrier_wait(args->barrier);
    while (*args->ongoing) {
        MCTS::evaluateMoves(args->board, args->visits, args->numIterations, &seed);
        pthread_barrier_wait(args->barrier);
        /* wait for master thread to find out and apply the chosen move */
        pthread_barrier_wait(args->barrier);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    int numProc, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &numProc);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    unsigned int seed = time(NULL) + rank * MAX_THREADS;
    Board board;
    bool ongoing;
    int boardSize = BOARD_SIZE;
    int maxMoves = boardSize * boardSize + 1;
    vector< atomic<int> > visits;
    int numThreads;
    if (argc > 1) {
        numThreads = atoi(argv[1]);
    }
    else {
        numThreads = get_nprocs();
    }
    
    int iterationsPerThread = ITERATIONS_PER_MOVE / (numProc * numThreads) + 1;
    pthread_t threads[numThreads-1];
    ThreadArgs threadArgs[numThreads-1];
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, numThreads);
    
    for (int i = 0; i < numThreads-1; ++i) {
        threadArgs[i].threadId = rank * MAX_THREADS + i + 1;
        threadArgs[i].board = &board;
        threadArgs[i].ongoing = &ongoing;
        threadArgs[i].visits = &visits;
        threadArgs[i].numIterations = iterationsPerThread;
        threadArgs[i].barrier = &barrier;
        pthread_create(&threads[i], NULL, threadFunction, &threadArgs[i]);
    }
    
    if (rank == 0) {
        int winsBlack = 0, winsWhite = 0;
        int *visitsRecv = new int[maxMoves];
        board = Board(boardSize);
        ongoing = true;
        visits = vector< atomic<int> >(maxMoves);
        pthread_barrier_wait(&barrier);
        for (int i = 0; i < NUM_GAMES; ++i) {
            while (true) {
                MCTS::evaluateMoves(&board, &visits, iterationsPerThread, &seed);
                pthread_barrier_wait(&barrier);
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
                    board.applyMove(Position((chosenMove-1)/boardSize, (chosenMove-1)%boardSize));
                }
                ongoing = board.isOngoing();
                if (ongoing) {
                    visits = vector< atomic<int> >(maxMoves);
                    pthread_barrier_wait(&barrier);
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
                        board = Board(boardSize);
                        ongoing = true;
                        visits = vector< atomic<int> >(maxMoves);
                    }
                    pthread_barrier_wait(&barrier);
                    break;
                }
            }
        }
        cout << "Black " << winsBlack << " - " << winsWhite << " White\n";
        delete[] visitsRecv;
    }
    else {
        int *visitsSend;
        bool differentSizes = (sizeof(atomic<int>) != sizeof(int));
        if (differentSizes) {
            visitsSend = new int[maxMoves];
        }
        board = Board(boardSize);
        ongoing = true;
        visits = vector< atomic<int> >(maxMoves);
        int chosenMove;
        pthread_barrier_wait(&barrier);
        for (int i = 0; i < NUM_GAMES; ++i) {
            while (true) {
                MCTS::evaluateMoves(&board, &visits, iterationsPerThread, &seed);
                pthread_barrier_wait(&barrier);
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
                    board.applyMove(Position((chosenMove-1)/boardSize, (chosenMove-1)%boardSize));
                }
                ongoing = board.isOngoing();
                if (ongoing) {
                    visits = vector< atomic<int> >(maxMoves);
                    pthread_barrier_wait(&barrier);
                }
                else {
                    if (i < NUM_GAMES - 1) {
                        board = Board(boardSize);
                        ongoing = true;
                        visits = vector< atomic<int> >(maxMoves);
                    }
                    pthread_barrier_wait(&barrier);
                    break;
                }
            }
        }
        if (differentSizes) {
            delete[] visitsSend;
        }
    }

    for (int i = 0; i < numThreads-1; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    MPI_Finalize();
    return 0;
}
