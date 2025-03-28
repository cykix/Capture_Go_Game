#include "game.hpp"

Game::Game(const std::string& p1, const std::string& p2)
    : player1(p1), player2(p2), currentPlayer(p1), gameOver(false) {
    board.resize(49, -1); // Empty board
}

bool Game::makeMove(const std::string& player, int position) {
    if (gameOver || player != currentPlayer || position < 0 || position > 48 || board[position] != -1)
        return false;

    board[position] = (player == player1) ? 0 : 1;
    currentPlayer = (player == player1) ? player2 : player1;

    return true;
}

bool Game::isGameOver() const {
    return gameOver;
}

std::string Game::getWinner() const {
    return winner;
}
