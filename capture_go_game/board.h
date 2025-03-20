#ifndef BOARD_H
#define BOARD_H

#include <iostream>
#include <vector>
#include <set>

using namespace std;

class Board {
public:
    static const int SIZE = 7;
    char grid[SIZE][SIZE];

    Board();

    void printBoard();
    bool isValidMove(int row, int col);
    void makeMove(int row, int col, char player);
    bool isCaptured(int row, int col, char player);
    void clearBoard();

    bool hasLiberties(int row, int col, char player); // Check if a stone/group has liberties
    void removeCapturedStones(char player);
    void floodFill(int row, int col, char player, set<pair<int, int>>& visited);
    string getBoardState();

private:
    bool isInBounds(int row, int col);
};

#endif // BOARD_H
