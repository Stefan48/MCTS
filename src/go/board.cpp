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
    this->passes = other.passes;
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
        this->passes = other.passes;
    }
    return *this;
}

bool operator== (const Board &b1, const Board &b2)
{
    if(b1.boardSize != b2.boardSize) {
        return false;
    }
    for(int i = 0; i < b1.boardSize; i++){
        for(int j = 0; j < b1.boardSize; j++){
            if(b1.board[i][j] != b2.board[i][j]) {
                return false;
            }
        }
    }
    return true;
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
    if (passes < 2) {
        return ONGOING;
    }
    // TODO
    return rand() % 2 + 1;
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
    int status = getStatus();
    if (status == ONGOING) {
        cout << "Game in progress";
    }
    else if (status == P1) {
        cout << "Black won";
    }
    else if (status == P2) {
        cout << "White won";
    }
    cout << '\n';
}

bool Board::chainHasLiberties(int player, Position pos, bool** visited){
    //cout<<"Inside chainHasLiberties: player=" << player << " position=(" << pos.x << "," << pos.y << ")\n";
    if(pos.x < 0 || pos.y < 0 || pos.x >= this->boardSize || pos.y >= this->boardSize || visited[pos.x][pos.y]) {
        //cout<<"Return value in chainHasLiberties [player=" << player << " position=(" << pos.x << "," << pos.y << ")]: " << false << " (invalid or visited)\n";
        return false;
    }
    visited[pos.x][pos.y] = true;
    if(board[pos.x][pos.y] == Board::getOpponent(player)) {
        //cout<<"Return value in chainHasLiberties [player=" << player << " position=(" << pos.x << "," << pos.y << ")]: " << false << "(opponent)\n";
        return false;
    }
    if(board[pos.x][pos.y] == EMPTY) {
        //cout<<"Return value in chainHasLiberties [player=" << player << " position=(" << pos.x << "," << pos.y << ")]: " << true << "(empty)\n";
        return true;
    }


    bool ret = chainHasLiberties(player,Position(pos.x+1, pos.y), visited) ||
            chainHasLiberties(player, Position(pos.x, pos.y + 1), visited) ||
            chainHasLiberties(player, Position(pos.x-1, pos.y), visited) ||
            chainHasLiberties(player, Position(pos.x, pos.y-1), visited);\
    //out<<"Return value in chainHasLiberties [player=" << player << " position=(" << pos.x << "," << pos.y << ")]: " << ret << "\n";
    return ret;
}
bool Board::isSuicideMove(int player, Position pos){
    //cout<<"Entered isSuicideMove: player=" << player << " position=(" << pos.x << "," << pos.y << ")\n";
    bool** visited;
    visited = new bool*[boardSize];
    for (int i = 0; i < boardSize; ++i) {
        visited[i] = new bool[boardSize]();
    }

    visited[pos.x][pos.y] = true;
    int opponent = Board::getOpponent(player);

    /* bool temp1 = !chainHasLiberties(player,Position(pos.x+1, pos.y), visited);
    bool temp2 = !chainHasLiberties(player, Position(pos.x, pos.y + 1), visited);
    bool temp3 = !chainHasLiberties(player, Position(pos.x-1, pos.y), visited);
    bool temp4 = !chainHasLiberties(player, Position(pos.x, pos.y-1), visited);
    bool isSuicide = temp1 && temp2 && temp3 && temp4; */

    bool isSuicide = !chainHasLiberties(player,Position(pos.x+1, pos.y), visited) &&
            !chainHasLiberties(player, Position(pos.x, pos.y + 1), visited) &&
            !chainHasLiberties(player, Position(pos.x-1, pos.y), visited) &&
            !chainHasLiberties(player, Position(pos.x, pos.y-1), visited);
    // cout<<"Inside isSuicideMove [player=" << player << " position=(" << pos.x << "," << pos.y << ")]: isSuicide=" << isSuicide << "\n";
    for (int i = 0; i < boardSize; ++i){
        for (int j = 0; j < boardSize; ++j) {
            visited[i][j] = false;
        }
    }
    visited[pos.x][pos.y] = true;
    bool capture = ((pos.x + 1 < boardSize) && board[pos.x+1][pos.y] == opponent && !chainHasLiberties(opponent,Position(pos.x+1, pos.y), visited)) ||
            ((pos.y + 1 < boardSize) && board[pos.x][pos.y+1] == opponent && !chainHasLiberties(opponent, Position(pos.x, pos.y+1), visited)) ||
            ((pos.x - 1 >= 0) && board[pos.x-1][pos.y] == opponent && !chainHasLiberties(opponent, Position(pos.x-1, pos.y), visited)) ||
            ((pos.y - 1 >= 0) && board[pos.x][pos.y-1] == opponent && !chainHasLiberties(opponent, Position(pos.x, pos.y-1), visited));
    // cout<<"Inside isSuicideMove [player=" << player << " position=(" << pos.x << "," << pos.y << ")]: isCapture=" << capture << "\n";
    for (int i = 0; i < boardSize; ++i) {
        delete[] visited[i];
    }
    delete[] visited;

    return isSuicide && !capture;

}

 bool Board::isKoMove(int playerTurn, Position pos){
    Board copyBoard = Board(*this);
    copyBoard.applyMove(playerTurn, pos);
    if(prevBoard && copyBoard == *prevBoard) {
        return true;
    }
    return false;
 }
vector<Position> Board::getValidMoves(int playerTurn) {
    vector<Position> moves;
    if (this->passes < 2) {
        moves.push_back(Position(-1, -1));
    }
    else {
        //printBoard();
        //this->printStatus();
        return moves;
    }
    for (int i = 0; i < boardSize; ++i) {
        for (int j = 0; j < boardSize; ++j) {
            if (board[i][j] == EMPTY && !isSuicideMove(playerTurn, Position(i, j)) && !isKoMove(playerTurn, Position(i, j))) {
                moves.push_back(Position(i, j));
            }
        }
    }
    if(moves.size() == 0){
        printBoard();
    }
    return moves;
}
void Board::capturePieces(int playerCaptured, Position pos, bool** visited){
    for (int i = 0; i < boardSize; ++i){
        for (int j = 0; j < boardSize; ++j) {
            if(board[i][j] == playerCaptured && visited[i][j]){
                board[i][j] = EMPTY;
            }
        }
    }

}
void Board::applyMove(int player, Position pos) {
    /* assume the move is valid */
    if(prevBoard) {
        for (int i = 0; i < prevBoard->boardSize; ++i) {
            delete[] prevBoard->board[i];
        }
        delete[] prevBoard->board;
    }
    prevBoard = new Board(*this);
    if (pos.x == -1 && pos.y == -1) {
        this->passes++;
        return;
    }
    board[pos.x][pos.y] = player;

    bool** visited;
    visited = new bool*[boardSize];
    for (int i = 0; i < boardSize; ++i) {
        visited[i] = new bool[boardSize]();
    }

    visited[pos.x][pos.y] = true;
    int opponent = Board::getOpponent(player);
    bool captured = false;
    if(pos.x + 1 < this->boardSize && board[pos.x+1][pos.y] == opponent){
        captured = !chainHasLiberties(opponent, Position(pos.x+1, pos.y), visited);
    }
    if(captured){
        capturePieces(opponent, Position(pos.x+1, pos.y), visited);
    }
    for (int i = 0; i < boardSize; ++i){
       memset(visited[i], 0, boardSize*sizeof(bool));
    }
    if(pos.y + 1 < this->boardSize && board[pos.x][pos.y + 1] == opponent){
        captured = !chainHasLiberties(opponent, Position(pos.x, pos.y + 1), visited);
    }
    if(captured){
        capturePieces(opponent, Position(pos.x, pos.y + 1), visited);
    }
    for (int i = 0; i < boardSize; ++i){
        memset(visited[i], 0, boardSize*sizeof(bool));
    }
    if(pos.x - 1 >= 0 && board[pos.x - 1][pos.y] == opponent){
        captured = !chainHasLiberties(opponent, Position(pos.x -1, pos.y), visited);
    }
    if(captured){
       capturePieces(opponent, Position(pos.x-1, pos.y), visited);
    }
    for (int i = 0; i < boardSize; ++i){
        memset(visited[i], 0, boardSize*sizeof(bool));
    }
    if(pos.y - 1 >= 0 && board[pos.x][pos.y - 1] == opponent){
        captured = !chainHasLiberties(opponent, Position(pos.x, pos.y - 1), visited);
    }
    if(captured){
        capturePieces(opponent, Position(pos.x, pos.y-1), visited);
    }

    for (int i = 0; i < boardSize; ++i) {
        delete[] visited[i];
    }
    delete[] visited;
}


