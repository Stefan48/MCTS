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
        memcpy(this->board[i], board[i], boardSize * sizeof(int));
    }
}

Board::Board(int boardSize, int **board, int playerTurn) {
    this->boardSize = boardSize;
    this->board = new int*[boardSize];
    for (int i = 0; i < boardSize; ++i) {
        this->board[i] = new int[boardSize];
        memcpy(this->board[i], board[i], boardSize * sizeof(int));
    }
    this->playerTurn = playerTurn;
}

Board::Board(const Board &other) {
    this->boardSize = other.boardSize;
    this->board = new int*[this->boardSize];
    for (int i = 0; i < this->boardSize; ++i) {
        this->board[i] = new int[this->boardSize];
        memcpy(this->board[i], other.board[i], boardSize * sizeof(int));
    }
    this->playerTurn = other.playerTurn;
    if (other.prevBoard) {
        this->prevBoard = new int*[this->boardSize];
        for (int i = 0; i < this->boardSize; ++i) {
            this->prevBoard[i] = new int[this->boardSize];
            memcpy(this->prevBoard[i], other.prevBoard[i], boardSize * sizeof(int));
        }
    }
    else {
        this->prevBoard = NULL;
    }
    this->passes = other.passes;
}

Board::~Board() {
    for (int i = 0; i < boardSize; ++i) {
        delete[] board[i];
    }
    delete[] board;
    if (prevBoard) {
        for (int i = 0; i < boardSize; ++i) {
            delete[] prevBoard[i];
        }
        delete[] prevBoard;
    }
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
            memcpy(this->board[i], other.board[i], boardSize * sizeof(int));
        }
        this->playerTurn = other.playerTurn;
        if (this->prevBoard) {
            for (int i = 0; i < this->boardSize; ++i) {
                delete[] this->prevBoard[i];
            }
            delete[] this->prevBoard;
        }
        if (other.prevBoard) {
            this->prevBoard = new int*[this->boardSize];
            for (int i = 0; i < this->boardSize; ++i) {
                this->prevBoard[i] = new int[this->boardSize];
                memcpy(this->prevBoard[i], other.prevBoard[i], boardSize * sizeof(int));
            }
        }
        else {
            this->prevBoard = NULL;
        }
        this->passes = other.passes;
    }
    return *this;
}

bool Board::equalBoards(int **b1, int **b2, int boardSize) {
    for (int i = 0; i < boardSize; ++i) {
        if (memcmp(b1[i], b2[i], boardSize * sizeof(int))) {
            return false;
        }
    }
    return true;
}

bool operator==(const Board &b1, const Board &b2)
{
    if (b1.boardSize != b2.boardSize) {
        return false;
    }
    return Board::equalBoards(b1.board, b2.board, b1.boardSize);
}

int** Board::getBoard() {
    return board;
}

void Board::setBoard(int **board) {
    this->board = board;
}

int Board::getBoardSize() {
    return boardSize;
}


int Board::getPlayerTurn() {
    return playerTurn;
}

void Board::togglePlayerTurn() {
    playerTurn = getCurrentOpponent();
}

int Board::getCurrentOpponent() {
    return Board::getOpponent(playerTurn);
}

bool Board::isOngoing() {
    return (passes < 2);
}

void Board::updatePlayersFound(int &currentPlayersFound, int newPlayersFound) {
    if (currentPlayersFound == P1 + P2) {
        return;
    }
    if (newPlayersFound == P1 + P2) {
        currentPlayersFound = newPlayersFound;
        return;
    }
    if (currentPlayersFound == P1) {
        if (newPlayersFound == P2) {
            currentPlayersFound = P1 + P2;
        }
    }
    else if (currentPlayersFound == P2) {
        if (newPlayersFound == P1) {
            currentPlayersFound = P1 + P2;
        }
    }
    else {
        currentPlayersFound = newPlayersFound;
    }
}

int Board::countTerritory(Position pos, bool **visited, int &cnt) {
    if (visited[pos.x][pos.y]) {
        return board[pos.x][pos.y];
    }
    visited[pos.x][pos.y] = true;
    if (board[pos.x][pos.y] == EMPTY) {
        cnt++;
        int playersFound = 0;
        if (pos.x+1 < boardSize) {
            updatePlayersFound(playersFound, countTerritory(Position(pos.x+1, pos.y), visited, cnt));
        }
        if (pos.y+1 < boardSize) {
            updatePlayersFound(playersFound, countTerritory(Position(pos.x, pos.y+1), visited, cnt));
        }
        if (pos.x-1 >= 0) {
            updatePlayersFound(playersFound, countTerritory(Position(pos.x-1, pos.y), visited, cnt));
        }
        if (pos.y-1 >= 0) {
            updatePlayersFound(playersFound, countTerritory(Position(pos.x, pos.y-1), visited, cnt));
        }
        return playersFound;
    }
    else if (board[pos.x][pos.y] == P1) {
        return P1;
    }
    else {
        return P2;
    }
}

int Board::getStatus(float *blackScore, float *whiteScore) {
    if (passes < 2) {
        return ONGOING;
    }
    /* this is an optimization in order to not recompute the score
     * when performing multiple simulations on the same final state */
    if (winner != ONGOING && !blackScore && !whiteScore) {
        return winner;
    }
    float black = 0, white = KOMI;
    int playersFound, cnt;
    bool **visited = new bool*[boardSize];
    for (int i = 0; i < boardSize; ++i) {
        visited[i] = new bool[boardSize]();
    }
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == P1) {
                black++;
            }
            else if (board[i][j] == P2) {
                white++;
            }
            else if (!visited[i][j]) {
                cnt = 0;
                playersFound = countTerritory(Position(i, j), visited, cnt);
                if (playersFound == P1) {
                    black += cnt;
                }
                else if (playersFound == P2) {
                    white += cnt;
                }
            }
        }
    }
    for (int i = 0; i < boardSize; ++i) {
        delete[] visited[i];
    }
    delete[] visited;
    if (blackScore) {
        *blackScore = black;
    }
    if (whiteScore) {
        *whiteScore = white;
    }
    winner = black > white ? P1 : P2;
    return winner;
}

void Board::printBoard() {
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == EMPTY) {
                cout << '_';
            }
            else if (board[i][j] == P1) {
                cout << 'B';
            }
            else {
                cout << 'W';
            }
            cout << ' ';
        }
        cout << '\n';
    }
    cout << '\n';
}

void Board::printStatus() {
    float blackScore, whiteScore;
    int status = getStatus(&blackScore, &whiteScore);
    if (status == ONGOING) {
        cout << "Game in progress";
    }
    else if (status == P1) {
        cout << "Black won (" << blackScore << " - " << whiteScore << ')';
    }
    else if (status == P2) {
        cout << "White won (" << blackScore << " - " << whiteScore << ')';
    }
    cout << '\n';
}

bool Board::chainHasLiberties(int player, Position pos, bool **visited) {
    /* assume the position is valid */
    if (board[pos.x][pos.y] == Board::getOpponent(player)) {
        return false;
    }
    if (board[pos.x][pos.y] == EMPTY) {
        return true;
    }
    visited[pos.x][pos.y] = true;
    bool hasLiberties = false;
    if (pos.x+1 < boardSize && !visited[pos.x+1][pos.y]) {
        hasLiberties = chainHasLiberties(player,Position(pos.x+1, pos.y), visited);
    }
    if (pos.y+1 < boardSize && !visited[pos.x][pos.y+1]) {
        hasLiberties = chainHasLiberties(player, Position(pos.x, pos.y+1), visited) || hasLiberties;
    }
    if (pos.x-1 >= 0 && !visited[pos.x-1][pos.y]) {
        hasLiberties = chainHasLiberties(player, Position(pos.x-1, pos.y), visited) || hasLiberties;
    }
    if (pos.y-1 >= 0 && !visited[pos.x][pos.y-1]) {
        hasLiberties = chainHasLiberties(player, Position(pos.x, pos.y-1), visited) || hasLiberties;
    }
    return hasLiberties;
}

bool Board::isSuicideMove(Position pos) {
    int player = this->playerTurn;
    bool **visited;
    visited = new bool*[boardSize];
    for (int i = 0; i < boardSize; ++i) {
        visited[i] = new bool[boardSize]();
    }
    visited[pos.x][pos.y] = true;
    bool isSuicide = true;
    if (pos.x+1 < boardSize) {
        isSuicide = !chainHasLiberties(player, Position(pos.x+1, pos.y), visited);
    }
    if (pos.y+1 < boardSize && !visited[pos.x][pos.y+1]) {
        isSuicide = isSuicide && !chainHasLiberties(player, Position(pos.x, pos.y+1), visited);
    }
    if (pos.x-1 >= 0 && !visited[pos.x-1][pos.y]) {
        isSuicide = isSuicide && !chainHasLiberties(player, Position(pos.x-1, pos.y), visited);
    }
    if (pos.y-1 >= 0 && !visited[pos.x][pos.y-1]) {
        isSuicide = isSuicide && !chainHasLiberties(player, Position(pos.x, pos.y-1), visited);
    }
    for (int i = 0; i < boardSize; ++i){
        memset(visited[i], 0, boardSize * sizeof(bool));
    }
    visited[pos.x][pos.y] = true;
    int opponent = this->getCurrentOpponent();
    bool isCapture = false;
    if (pos.x+1 < boardSize && board[pos.x+1][pos.y] == opponent) {
        isCapture = !chainHasLiberties(opponent, Position(pos.x+1, pos.y), visited);
    }
    if (pos.y+1 < boardSize && board[pos.x][pos.y+1] == opponent && !visited[pos.x][pos.y+1]) {
        isCapture = isCapture || !chainHasLiberties(opponent, Position(pos.x, pos.y+1), visited);
    }
    if (pos.x-1 >= 0 && board[pos.x-1][pos.y] == opponent && !visited[pos.x-1][pos.y]) {
        isCapture = isCapture || !chainHasLiberties(opponent, Position(pos.x-1, pos.y), visited);
    }
    if (pos.y-1 >= 0 && board[pos.x][pos.y-1] == opponent && !visited[pos.x][pos.y-1]) {
        isCapture = isCapture || !chainHasLiberties(opponent, Position(pos.x, pos.y-1), visited);
    }
    for (int i = 0; i < boardSize; ++i) {
        delete[] visited[i];
    }
    delete[] visited;
    return isSuicide && !isCapture;
}

bool Board::isKoMove(Position pos) {
    if (this->prevBoard == NULL) {
        return false;
    }
    Board tempBoard = Board(*this);
    tempBoard.applyMove(pos);
    return equalBoards(tempBoard.board, this->prevBoard, this->boardSize);
}

vector<Position> Board::getValidMoves() {
    vector<Position> moves;
    if (this->passes >= 2) {
        return moves;
    }
    moves.push_back(Position(-1, -1));
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == EMPTY && !isSuicideMove(Position(i, j)) && !isKoMove(Position(i, j))) {
                moves.push_back(Position(i, j));
            }
        }
    }
    return moves;
}

void Board::capturePieces(int playerCaptured, bool **visited) {
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == playerCaptured && visited[i][j]){
                board[i][j] = EMPTY;
            }
        }
    }
}

void Board::applyMove(Position pos) {
    /* assume the move is valid */
    if (prevBoard) {
        for (int i = 0; i < boardSize; ++i) {
            memcpy(prevBoard[i], board[i], boardSize * sizeof(int));
        }
    }
    else {
        prevBoard = new int*[boardSize];
        for (int i = 0; i < boardSize; ++i) {
            prevBoard[i] = new int[boardSize];
            memcpy(prevBoard[i], board[i], boardSize * sizeof(int));
        }
    }

    if (pos.x == -1 && pos.y == -1) {
        this->passes++;
        this->togglePlayerTurn();
        return;
    }
    this->passes = 0;
    board[pos.x][pos.y] = this->playerTurn;
    bool **visited;
    visited = new bool*[boardSize];
    for (int i = 0; i < boardSize; ++i) {
        visited[i] = new bool[boardSize]();
    }
    visited[pos.x][pos.y] = true;
    int opponent = this->getCurrentOpponent();
    if (pos.x+1 < boardSize && board[pos.x+1][pos.y] == opponent) {
        if (!chainHasLiberties(opponent, Position(pos.x+1, pos.y), visited)) {
            capturePieces(opponent, visited);
        }
        for (int i = 0; i < boardSize; ++i) {
            memset(visited[i], 0, boardSize * sizeof(bool));
        }
        visited[pos.x][pos.y] = true;
    }
    if (pos.y+1 < boardSize && board[pos.x][pos.y+1] == opponent) {
        if (!chainHasLiberties(opponent, Position(pos.x, pos.y+1), visited)) {
            capturePieces(opponent, visited);
        }
        for (int i = 0; i < boardSize; ++i) {
            memset(visited[i], 0, boardSize * sizeof(bool));
        }
        visited[pos.x][pos.y] = true;
    }
    if (pos.x-1 >= 0 && board[pos.x-1][pos.y] == opponent) {
        if (!chainHasLiberties(opponent, Position(pos.x -1, pos.y), visited)) {
            capturePieces(opponent, visited);
        }
        for (int i = 0; i < boardSize; ++i) {
            memset(visited[i], 0, boardSize * sizeof(bool));
        }
        visited[pos.x][pos.y] = true;
    }
    if (pos.y-1 >= 0 && board[pos.x][pos.y-1] == opponent) {
        if (!chainHasLiberties(opponent, Position(pos.x, pos.y - 1), visited)) {
            capturePieces(opponent, visited);
        }
    }
    for (int i = 0; i < boardSize; ++i) {
        delete[] visited[i];
    }
    delete[] visited;
    this->togglePlayerTurn();
}

void Board::applyRandomMove(unsigned int *seed) {
    vector<Position> moves = getValidMoves();
    int chosen = rand_r(seed) % moves.size();
    applyMove(moves[chosen]);
    this->togglePlayerTurn();
}
