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
    pthread_barrier_wait(args->barrier);
    while (*args->ongoing) {
        MCTS::evaluateMoves(args->board, args->visits, args->numIterations);
        //cout << "Thread " << args->threadId << " evaluated\n";
        pthread_barrier_wait(args->barrier);
        /* wait for master thread to decide and apply the overall best move */
        pthread_barrier_wait(args->barrier);
    }
    pthread_exit(NULL);
}

int main()
{
    // TODO replace 'srand'
    srand(time(NULL));
    
    Board board;
    bool ongoing;
    int maxMoves = BOARD_SIZE * BOARD_SIZE + 1;
    vector< atomic<int> > visits;
    
    int numThreads = get_nprocs();
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
    board = Board(BOARD_SIZE);
    ongoing = true;
    visits = vector< atomic<int> >(maxMoves);
    pthread_barrier_wait(&barrier);
    for (int i = 0; i < NUM_GAMES; ++i) {
        while (true) {
            MCTS::evaluateMoves(&board, &visits, iterationsPerThread);
            //cout << "Master evaluated\n";
            pthread_barrier_wait(&barrier);
            
            /*for (int i = 0; i < maxMoves; ++i)
                cout << visits[i] << " ";
            cout << "\n~~~~~~~~~\n";*/
            
            int bestMove, maxVisits = -1;
            for (int j = 0; j < (int)visits.size(); ++j) {
                if (visits[j] >= maxVisits) {
                    maxVisits = visits[j];
                    bestMove = j;
                }
            }
            Position pos;
            if (bestMove == 0) {
                pos = Position(-1, -1);
            }
            else {
                pos = Position((bestMove-1) / board.getBoardSize(), (bestMove-1) % board.getBoardSize());
            }
            //cout << pos.x << " " << pos.y << "\n";
            board.applyMove(pos);
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
                    board = Board(BOARD_SIZE);
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
