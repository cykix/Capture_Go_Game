#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>
#include "protocol.hpp"

class Client {
public:
    Client(const std::string& serverIP, int serverPort);
    
    void start();  // ✅ Ensures the client has a main loop for handling input/output
    void connectToServer();
    void sendMessage(const std::string& message);
    void receiveMessage();

private:
    std::string serverIP;
    int serverPort;
    int sockfd; // ✅ Socket file descriptor for network communication

    void handleServerResponse(const std::string& response); // ✅ Handles protocol responses
};

#endif // CLIENT_HPP
