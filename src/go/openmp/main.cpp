#include <bits/stdc++.h>
#include <omp.h>
#include <sys/sysinfo.h>
#include "position.h"
#include "board.h"
#include "node.h"
#include "mcts.h"
using namespace std;

#define BOARD_SIZE 9
#define NUM_GAMES 10
#define ITERATIONS_PER_MOVE 100

int main(int argc, char *argv[]) {
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
    omp_set_num_threads(numThreads);
    int iterationsPerThread = ITERATIONS_PER_MOVE / numThreads + 1;

    #pragma omp parallel default(none) shared(board, ongoing, boardSize, visits, iterationsPerThread, maxMoves, cout)
    {
        int threadId = omp_get_thread_num();
        unsigned int seed = time(NULL) + threadId;
        if (threadId == 0) {
            int winsBlack = 0, winsWhite = 0;
            board = Board(boardSize);
            ongoing = true;
            visits = vector< atomic<int> >(maxMoves);
            #pragma omp barrier
            for (int i = 0; i < NUM_GAMES; ++i) {
                while (true) {
                    MCTS::evaluateMoves(&board, &visits, iterationsPerThread, &seed);
                    #pragma omp barrier
                    int chosenMove, maxVisits = -1;
                    for (int j = 0; j < (int)visits.size(); ++j) {
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
                            board = Board(boardSize);
                            ongoing = true;
                            visits = vector< atomic<int> >(maxMoves);
                        }
                        #pragma omp barrier
                        break;
                    }
                }
            }
            cout << "Black " << winsBlack << " - " << winsWhite << " White\n";
        }
        else {
            #pragma omp barrier
            while (ongoing) {
                MCTS::evaluateMoves(&board, &visits, iterationsPerThread, &seed);
                #pragma omp barrier
                /* wait for master thread to decide and apply the chosen move */
                #pragma omp barrier
            }
        }
    }
    return 0;
}
