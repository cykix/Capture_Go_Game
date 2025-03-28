#ifndef GAME_HPP
#define GAME_HPP

#include <string>
#include <vector>

class Game {
public:
    Game(const std::string& player1, const std::string& player2);
    
    bool makeMove(const std::string& player, int position);
    bool isGameOver() const;
    std::string getWinner() const;

private:
    std::string player1, player2, currentPlayer;
    std::vector<int> board;
    bool gameOver;
    std::string winner;
};

#endif // GAME_HPP
