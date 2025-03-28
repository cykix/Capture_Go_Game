#ifndef SERVER_HPP
#define SERVER_HPP

#include <string>
#include <set>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <netinet/in.h>  // For sockaddr_in
#include <thread>        // For handling clients in separate threads
#include "game.hpp"      // Include game class
#include "protocol.hpp"  // Include protocol for handling messages

class Server {
public:
    Server(int port); // Constructor to initialize server with port
    ~Server();        // Destructor to clean up resources
    
    void start();     // Start the server and listen for client connections
    void stop();      // Stop the server

private:
    int serverPort;                                     // Port the server listens on
    bool isRunning;                                     // Server state (running or not)
    std::set<std::string> clients;                      // Set of logged-in clients
    std::vector<std::string> queue;                     // Queue for matching players
    std::unordered_map<std::string, std::shared_ptr<Game>> games; // Active games
    std::mutex serverMutex;                             // Mutex for thread safety

    // Handle each client connection in a separate thread
    void handleClient(int clientSocket);
    
    // Process specific protocol commands
    void handleHello(int clientSocket, const std::vector<std::string>& args);
    void handleLogin(int clientSocket, const std::vector<std::string>& args);
    void handleList(int clientSocket);
    void handleQueue(int clientSocket, const std::vector<std::string>& args);
    void handleMove(int clientSocket, const std::vector<std::string>& args);
    
    // Start a game with two players
    void startGame(const std::string& player1, const std::string& player2);
    
    // Send a message to a client (need proper client connection tracking)
    void sendMessageToClient(const std::string& username, const std::string& message);
    
};

#endif // SERVER_HPP
