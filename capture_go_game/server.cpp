#include "server.hpp"
#include "protocol.hpp"
#include "game.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <vector>

Server::Server(int port) : serverPort(port), isRunning(false) {}

Server::~Server() {
    stop();
}

void Server::start() {
    isRunning = true;

    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        perror("ERROR: Could not create socket");
        return;
    }

    // Set up server address struct
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(serverPort);

    // Bind socket to the port
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("ERROR: Bind failed");
        close(serverSocket);
        return;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) < 0) {
        perror("ERROR: Listen failed");
        close(serverSocket);
        return;
    }

    std::cout << "Server started on port " << serverPort << " and listening...\n";

    while (isRunning) {
        sockaddr_in clientAddr;
        socklen_t clientLen = sizeof(clientAddr);

        // Accept a new client
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
        if (clientSocket < 0) {
            perror("ERROR: Accept failed");
            continue;
        }

        std::thread(&Server::handleClient, this, clientSocket).detach();
    }

    close(serverSocket);
}

void Server::stop() {
    isRunning = false;
    std::cout << "Server shutting down...\n";
}

void Server::handleClient(int clientSocket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (bytesReceived <= 0) {
            std::cout << "Client disconnected.\n";
            close(clientSocket);
            return;
        }

        std::string message(buffer);
        std::string command;
        std::vector<std::string> args;

        if (!Protocol::parseMessage(message, command, args)) {
            send(clientSocket, "ERROR~Invalid Message Format\n", 29, 0);
            continue;
        }

        if (command == Protocol::HELLO) {
            send(clientSocket, "HELLO~Server~RANK~CHAT\n", 24, 0);
        } else if (command == Protocol::LOGIN) {
            handleLogin(clientSocket, args);
        } else if (command == Protocol::LIST) {
            handleList(clientSocket);
        } else if (command == Protocol::QUEUE) {
            handleQueue(clientSocket, args);
        } else if (command == Protocol::MOVE) {
            handleMove(clientSocket, args);
        } else {
            send(clientSocket, "ERROR~Unknown Command\n", 23, 0);
        }
    }
}

void Server::handleLogin(int clientSocket, const std::vector<std::string>& args) {
    if (args.empty()) {
        const std::string message = "ERROR~Username Missing";
        send(clientSocket, message.c_str(), message.length(), 0);  // Send the message
        return;
    }

    std::string username = args[0];  // Correctly access the first argument for the username

    std::lock_guard<std::mutex> lock(serverMutex);
    if (clients.find(username) != clients.end()) {
        const std::string message = "ALREADYLOGGEDIN";
        send(clientSocket, message.c_str(), message.length(), 0);  // Send the already logged in message
    } else {
        clients.insert(username);  // Add the username to the set of logged-in users
        const std::string message = "LOGIN";  // Send login success message
        send(clientSocket, message.c_str(), message.length(), 0);  // Send the login success message
    }
}

void Server::handleList(int clientSocket) {
    std::string response = "LIST";
    for (const auto& user : clients) {
        response += "~" + user;
    }
    response += "\n";
    send(clientSocket, response.c_str(), response.length(), 0);
}

void Server::handleQueue(int clientSocket, const std::vector<std::string>& args) {
    if (args.empty()) {
        send(clientSocket, "ERROR~Username Missing\n", 24, 0);
        return;
    }

    std::string username = args[0];

    std::lock_guard<std::mutex> lock(serverMutex);
    if (std::find(queue.begin(), queue.end(), username) != queue.end()) {
        queue.erase(std::remove(queue.begin(), queue.end(), username), queue.end());
    } else {
        queue.push_back(username);
        if (queue.size() >= 2) {
            startGame(queue[0], queue[1]);
            queue.erase(queue.begin(), queue.begin() + 2);
        }
    }
}

void Server::startGame(const std::string& player1, const std::string& player2) {
    std::shared_ptr<Game> newGame = std::make_shared<Game>(player1, player2);
    games[player1] = newGame;
    games[player2] = newGame;

    std::string newGameMessage = "NEWGAME~" + player1 + "~" + player2 + "\n";
    sendMessageToClient(player1, newGameMessage);
    sendMessageToClient(player2, newGameMessage);
}

void Server::handleMove(int clientSocket, const std::vector<std::string>& args) {
    if (args.size() < 2) {
        send(clientSocket, "ERROR~Invalid MOVE Format\n", 27, 0);
        return;
    }

    std::string username = args[0];
    int position = std::stoi(args[1]);

    std::lock_guard<std::mutex> lock(serverMutex);
    if (games.find(username) != games.end()) {
        std::shared_ptr<Game> game = games[username];
        if (game->makeMove(username, position)) {
            std::string moveMessage = "MOVE~" + std::to_string(position) + "\n";
            sendMessageToClient(username, moveMessage);

            if (game->isGameOver()) {
                std::string gameOverMessage = "GAMEOVER~VICTORY~" + game->getWinner() + "\n";
                sendMessageToClient(game->getWinner(), gameOverMessage);
                games.erase(game->getWinner());
            }
        } else {
            send(clientSocket, "ERROR~Invalid move\n", 21, 0);
        }
    } else {
        send(clientSocket, "ERROR~Not in a game\n", 22, 0);
    }
}

void Server::sendMessageToClient(const std::string& username, const std::string& message) {
    // This function should send messages to clients (needs a client-tracking mechanism)
    std::cout << "Sending to " << username << ": " << message;
}

int main() {
    Server server(12345);
    server.start();
    return 0;
}
