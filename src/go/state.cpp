#include "state.h"

State::State() {}

State::State(Board board) {
    this->board = board;
}

State::State(Board board, int playerTurn) {
    this->board = board;
    this->playerTurn = playerTurn;
}

Board State::getBoard() {
    return board;
}

void State::setBoard(Board board) {
    this->board = board;
}

int State::getPlayerTurn() {
    return playerTurn;
}

void State::setPlayerTurn(int playerTurn) {
    this->playerTurn = playerTurn;
}

int State::getOpponent() {
    return Board::getOpponent(playerTurn);
}

void State::togglePlayerTurn() {
    playerTurn = getOpponent();
}

int State::getWins() {
    return wins;
}

void State::incrementWins() {
    this->wins++;
}

int State::getVisits() {
    return visits;
}

void State::incrementVisits() {
    this->visits++;
}

vector<State> State::getPossibleStates() {
    vector<State> possibleStates;
    vector<Position> moves = board.getValidMoves(this->playerTurn);
    for (auto it = moves.begin(); it != moves.end(); ++it) {
        State newState(this->board, this->getOpponent());
        newState.board.applyMove(this->playerTurn, *it);
        possibleStates.push_back(newState);
    }
    return possibleStates;
}

void State::applyRandomMove() {
    vector<Position> moves = board.getValidMoves(this->playerTurn);
    int chosen = rand() % moves.size();
    board.applyMove(this->playerTurn, moves[chosen]);
    this->togglePlayerTurn();
}
