#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <set>
#include <vector>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include "board.h"

using namespace std;

#define DEFAULT_PORT 7777
#define MAX_CLIENTS 2

class Server {
public:
    Server();
    void start();

private:
    int serverSocket;
    map<int, string> clientSockets;  // Maps socket to username
    set<string> usernames;
    vector<int> queue;
    mutex mtx;
    Board gameBoard;

    void handleClient(int clientSocket);
    void sendCommand(int clientSocket, const string& command);
    void processCommand(int clientSocket, const string& command);
    void startGame();
    void processMove(int clientSocket, const string& move);
    void gameOver(const string& reason, const string& winner = "");
    void sendBoardState(int clientSocket);
    int currentTurn;  // Keeps track of whose turn it is

};

#endif // SERVER_H
