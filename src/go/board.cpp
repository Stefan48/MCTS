#include "board.h"

int Board::getOpponent(int player) {
    return 3 - player;
}

Board::Board() {
    boardSize = DEFAULT_BOARD_SIZE;
    board = new int*[boardSize];
    for (int i = 0; i < boardSize; ++i) {
        board[i] = new int[boardSize]();
    }
}

Board::Board(int boardSize) {
    this->boardSize = boardSize;
    board = new int*[boardSize];
    for (int i = 0; i < boardSize; ++i) {
        board[i] = new int[boardSize]();
    }
}

Board::Board(int boardSize, int **board) {
    this->boardSize = boardSize;
    this->board = new int*[boardSize];
    for (int i = 0; i < boardSize; ++i) {
        this->board[i] = new int[boardSize];
        for (int j = 0; j < boardSize; ++j) {
            this->board[i][j] = board[i][j];
        }
    }
}

Board::Board(const Board &other) {
    this->boardSize = other.boardSize;
    this->board = new int*[this->boardSize];
    for (int i = 0; i < this->boardSize; ++i) {
        this->board[i] = new int[this->boardSize];
        for (int j = 0; j < this->boardSize; ++j) {
            this->board[i][j] = other.board[i][j];
        }
    }
}

Board::~Board() {
    for (int i = 0; i < boardSize; ++i) {
        delete[] board[i];
    }
    delete[] board;
}

Board& Board::operator=(const Board &other)
{
    if (this != &other)
    {
        for (int i = 0; i < this->boardSize; ++i) {
            delete[] this->board[i];
        }
        delete[] this->board;
        this->boardSize = other.boardSize;
        this->board = new int*[this->boardSize];
        for (int i = 0; i < this->boardSize; ++i) {
            this->board[i] = new int[this->boardSize];
            for (int j = 0; j < this->boardSize; ++j) {
                this->board[i][j] = other.board[i][j];
            }
        }
    }
    return *this;
}

int** Board::getBoard() {
    return board;
}

void Board::setBoard(int **board) {
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            this->board[i][j] = board[i][j];
        }
    }
}

int Board::getBoardSize() {
    return boardSize;
}

int Board::getStatus() {
    /* check lines */
    for (int i = 0; i < boardSize; ++i) {
        if (board[i][0] != EMPTY) {
            bool won = true;
            for (int j = 1; j < boardSize; ++j) {
                if (board[i][j] != board[i][j-1]) {
                    won = false;
                    break;
                }
            }
            if (won) {
                return board[i][0];
            }
        }
    }
    /* check columns */
    for (int j = 0; j < boardSize; ++j) {
        if (board[0][j] != EMPTY) {
            bool won = true;
            for (int i = 1; i < boardSize; ++i) {
                if (board[i][j] != board[i-1][j]) {
                    won = false;
                    break;
                }
            }
            if (won) {
                return board[0][j];
            }
        }
    }
    /* check diagonals */
    if (board[0][0] != EMPTY) {
        bool won = true;
        for (int i = 1; i < boardSize; ++i) {
            if (board[i][i] != board[i-1][i-1]) {
                won = false;
                break;
            }
        }
        if (won) {
            return board[0][0];
        }
    }
    if (board[0][boardSize-1] != EMPTY) {
        bool won = true;
        for (int i = 1; i < boardSize; ++i) {
            if (board[i][boardSize-i-1] != board[i-1][boardSize-i]) {
                won = false;
                break;
            }
        }
        if (won) {
            return board[0][boardSize-1];
        }
    }
    /* check for draw */
    bool gameEnd = true;
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == EMPTY) {
                gameEnd = false;
                break;
            }
        }
    }
    if (gameEnd) {
        return DRAW;
    }
    return ONGOING;
}

void Board::printBoard() {
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == EMPTY) {
                cout << '_';
            }
            else if (board[i][j] == P1) {
                cout << 'X';
            }
            else {
                cout << '0';
            }
            cout << ' ';
        }
        cout << '\n';
    }
    cout << '\n';
}

void Board::printStatus() {
    int status = getStatus();
    if (status == ONGOING) {
        cout << "Game in progress";
    }
    else if (status == P1) {
        cout << "X won";
    }
    else if (status == P2) {
        cout << "0 won";
    }
    else {
        cout << "Draw";
    }
    cout << '\n';
}

vector<Position> Board::getValidMoves() {
    vector<Position> moves;
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == EMPTY) {
                moves.push_back(Position(i, j));
            }
        }
    }
    return moves;
}

void Board::applyMove(int player, Position pos) {
    /* assume the move is valid */
    board[pos.x][pos.y] = player;
}


