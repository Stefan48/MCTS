#include <bits/stdc++.h>
#include <pthread.h>
#include <sys/sysinfo.h>
#include "position.h"
#include "board.h"
#include "node.h"
#include "mcts.h"
using namespace std;

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
        /* wait for master thread to decide and apply the chosen move */
        pthread_barrier_wait(args->barrier);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    unsigned int seed = time(NULL);
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
    int iterationsPerThread = ITERATIONS_PER_MOVE / numThreads + 1;
    pthread_t threads[numThreads-1];
    ThreadArgs threadArgs[numThreads-1];
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, numThreads);
    
    for (int i = 0; i < numThreads-1; ++i) {
        threadArgs[i].threadId = i + 1;
        threadArgs[i].board = &board;
        threadArgs[i].ongoing = &ongoing;
        threadArgs[i].visits = &visits;
        threadArgs[i].numIterations = iterationsPerThread;
        threadArgs[i].barrier = &barrier;
        pthread_create(&threads[i], NULL, threadFunction, &threadArgs[i]);
    }

    int winsBlack = 0, winsWhite = 0;
    board = Board(boardSize);
    ongoing = true;
    visits = vector< atomic<int> >(maxMoves);
    pthread_barrier_wait(&barrier);
    for (int i = 0; i < NUM_GAMES; ++i) {
        while (true) {
            MCTS::evaluateMoves(&board, &visits, iterationsPerThread, &seed);
            pthread_barrier_wait(&barrier); 
            int chosenMove, maxVisits = -1;
            for (int j = 0; j < maxMoves; ++j) {
                if (visits[j] > maxVisits) {
                    maxVisits = visits[j];
                    chosenMove = j;
                }
            }
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
    
    for (int i = 0; i < numThreads-1; ++i) {
        pthread_join(threads[i], NULL);
    }
    
    cout << "Black " << winsBlack << " - " << winsWhite << " White\n";

    return 0;
}
