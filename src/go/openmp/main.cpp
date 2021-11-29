#include <bits/stdc++.h>
#include <omp.h>
#include <sys/sysinfo.h>
#include "position.h"
#include "board.h"
#include "node.h"
#include "mcts.h"
using namespace std;

#define BOARD_SIZE 9
#define NUM_GAMES 1
#define ITERATIONS_PER_MOVE 1000


int main(int argc, char *argv[]) {
    Board board;
    bool ongoing;
    int maxMoves = BOARD_SIZE * BOARD_SIZE + 1;
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

    #pragma omp parallel default(none) shared(board, ongoing, visits, iterationsPerThread, maxMoves, cout)
    {
        int threadId = omp_get_thread_num();
        if (threadId == 0) {
            unsigned int seed = time(NULL);
            int winsBlack = 0, winsWhite = 0;
            board = Board(BOARD_SIZE);
            ongoing = true;
            visits = vector< atomic<int> >(maxMoves);
            #pragma omp barrier
            for (int i = 0; i < NUM_GAMES; ++i) {
                while (true) {
                    MCTS::evaluateMoves(&board, &visits, iterationsPerThread, &seed);
                    #pragma omp barrier
                    int bestMove, maxVisits = -1;
                    for (int j = 0; j < (int)visits.size(); ++j) {
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
                        pos = Position((bestMove-1) / board.getBoardSize(), (bestMove-1) % board.getBoardSize());
                    }
                    cout << pos.x << " " << pos.y << "\n";
                    board.applyMove(pos);
                    board.printBoard();
                    cout << "\n";
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
                            board = Board(BOARD_SIZE);
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
            unsigned int seed = time(NULL) + threadId;
            #pragma omp barrier
            while (ongoing) {
                MCTS::evaluateMoves(&board, &visits, iterationsPerThread, &seed);
                #pragma omp barrier
                /* wait for master thread to decide and apply the overall best move */
                #pragma omp barrier
            }
        }
    }
    return 0;
}
