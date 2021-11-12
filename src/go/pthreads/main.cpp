#include <bits/stdc++.h>
#include "position.h"
#include "board.h"
#include "node.h"
#include "mcts.h"
#include <pthread.h>

using namespace std;


struct ThreadParam {
    bool *ongoing;
    Board *board;
    vector< atomic<int> > *visits;
    pthread_barrier_t *barrier;
};

void *threadFunction(void *arg)
{
    ThreadParam threadParam = *(ThreadParam*) arg;
    while (*threadParam.ongoing) {
        pthread_barrier_wait(threadParam.barrier);
        MCTS::getNextMove(threadParam.board, threadParam.visits);
        pthread_barrier_wait(threadParam.barrier);
    }
    pthread_exit(NULL);
}

int main()
{
    srand(time(NULL));
    int numGames = 1;
    int NUM_THREADS = 7;
    int boardSize = 9;
    Board board(boardSize);
    bool ongoing = true;
    pthread_t ptid[NUM_THREADS];
    int maxMoves = boardSize * boardSize + 1;
    ThreadParam threadParams[NUM_THREADS];
    vector< atomic<int> > visits(maxMoves);
    pthread_barrier_t barrier;
    pthread_barrier_init(&barrier, NULL, NUM_THREADS + 1);
    int r;
    for (int i = 0; i < NUM_THREADS; i++) {
        threadParams[i].ongoing = &ongoing;
        threadParams[i].board = &board;
        threadParams[i].visits = &visits;
        threadParams[i].barrier = &barrier;
        r = pthread_create(&ptid[i], NULL, threadFunction, &threadParams[i]);
        if (r) {

            cout<<"Error on creating thread\n";
            return -1;
        }
    }

    while (numGames) {
        //board.printBoard();
        //cout << '\n';

        while (ongoing) {
            pthread_barrier_wait(&barrier);
            MCTS::getNextMove(&board, &visits);
            int bestMove, maxVisits = -1;
            for (int i = 0; i < (int)visits.size(); ++i) {
                if (visits[i] >= maxVisits) {
                    maxVisits = visits[i];
                    bestMove = i;
                }
            }
            Position pos;
            if (bestMove == 0) {
                pos = Position(-1, -1);
            }
            else {
                pos = Position((bestMove-1) / board.getBoardSize(), (bestMove-1) % board.getBoardSize());
            }
            cout << pos.x << ' ' << pos.y << '\n';
            board.applyMove(pos);
            //board.printBoard();
            //cout << '\n';
            ongoing = board.isOngoing();
            visits = vector< atomic<int> >(maxMoves);
            pthread_barrier_wait(&barrier);
        }
        //board.printStatus();
        //cout << '\n';
        numGames--;
    }



    /* srand(time(NULL));
    int draws = 0, winsP1 = 0, winsP2 = 0;
    for (int i = 0; i < 10; ++i) {
        Board board(13);
        int playerTurn = Board::P1;
        while (board.getStatus() == Board::ONGOING) {
            board = MCTS::getNextMove(board, playerTurn);
            playerTurn = Board::getOpponent(playerTurn);
        }
        int status = board.getStatus();
        if (status == Board::P1) {
            winsP1++;
        }
        else {
            winsP2++;
        }
    }
    cout << "draws: " << draws << "\nwins X: " << winsP1 << "\nwins 0: " << winsP2 << "\n"; */



    /* srand(time(NULL));
    int **v = new int*[5];
    for (int i = 0; i < 5; ++i) {
        v[i] = new int[5]();
    }
    v[0][0] = 0; v[0][1] = 1; v[0][2] = 0; v[0][3] = 0; v[0][4] = 1;
    v[1][0] = 1; v[1][1] = 0; v[1][2] = 1; v[1][3] = 1; v[1][4] = 0;
    v[2][0] = 0; v[2][1] = 1; v[2][2] = 2; v[2][3] = 2; v[2][4] = 1;
    v[3][0] = 1; v[3][1] = 2; v[3][2] = 0; v[3][3] = 2; v[3][4] = 2;
    v[4][0] = 2; v[4][1] = 0; v[4][2] = 2; v[4][3] = 0; v[4][4] = 0;
    Board board(5, v, Board::P1);
    board.printBoard();
    cout << '\n';
    vector<Position> moves = board.getValidMoves();
    for (unsigned int i = 0; i < moves.size(); ++i) {
        cout << moves[i].x << " " << moves[i].y << "\n";
    } */

    return 0;
}
