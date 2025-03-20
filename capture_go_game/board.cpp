#include "board.h"

using namespace std;

Board::Board(){
    clearBoard();
}

void Board::clearBoard(){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            grid[i][j] = '.';
        }
    }
}

void Board::printBoard(){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            cout << grid[i][j] << " ";
        }
        cout << endl;
    }
}

bool Board::isInBounds(int row, int col) {
    return row >= 0 && col >= 0 && row < SIZE && col < SIZE;
}

bool Board::isValidMove(int row, int col){
    if(!isInBounds(row, col)){
        return false;
    }
    return grid[row][col] == '.';
}

void Board::makeMove(int row, int col, char player){
    grid[row][col] = player;
}

bool Board::hasLiberties(int row, int col, char player){
    const int directions[4][2] = {{-1, 0}, {1,0}, {0,-1}, {0,1}};
    for(int i = 0; i < 4; i++){
        int newRow = row + directions[i][0];
        int newCol = col + directions[i][1];
        if(isInBounds(newRow, newCol) && grid[newRow][newCol] == '.'){
            return true;
        }
    }
    return false;
}

void Board::floodFill(int row, int col, char player, set<pair<int, int>>& visited){
    if(!isInBounds(row, col) || grid[row][col] != player || visited.count({row, col})){
        return;
    }

    visited.insert({row, col});

    const int directions[4][2] = {{-1,0}, {1,0}, {0,-1}, {0,1}};
    for(int i = 0; i < 4; i++){
        floodFill(row + directions[i][0], col + directions[i][1], player, visited);
    }
}

void Board::removeCapturedStones(char player){
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            if(grid[i][j] == player){
                if(!hasLiberties(i, j, player)){
                    grid[i][j] = '.';
                }
            }
        }
    }
}

bool Board::isCaptured(int row, int col, char player){
    set<pair<int, int>> visited;
    floodFill(row, col, player, visited);

    for(const auto& pos : visited){
        int r = pos.first;
        int c = pos.second;
        if (hasLiberties(r, c, player)){
            return false;
        }
    }

    return true;
}

string Board::getBoardState(){
    string state = "";
    for(int i = 0; i < SIZE; i++){
        for(int j = 0; j < SIZE; j++){
            state += grid[i][j];
        }
        state += "\n";
    }
    return state;
}

