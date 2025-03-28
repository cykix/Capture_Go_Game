#include "client.hpp"
#include <iostream>
#include <sstream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

Client::Client(const std::string& ip, int port) : serverIP(ip), serverPort(port), sockfd(-1) {}

void Client::start() {
    connectToServer();

    std::string command;
    while (true) {
        std::cout << "Enter command (HELLO, LOGIN <username>, LIST, QUEUE, MOVE <pos>, EXIT): ";
        std::getline(std::cin, command);

        if (command == "EXIT") break;  // Exit the loop if the user types 'EXIT'

        // Check if the command is LOGIN and ensure the username is provided
        if (command.rfind("LOGIN ", 0) == 0) {
            std::string username = command.substr(6);  // Extract username after "LOGIN "
            if (username.empty()) {
                std::cout << "ERROR: Please provide a username.\n";
                continue;
            }
            sendMessage("LOGIN~" + username);  // Send the LOGIN command with the username
        } else if (command == "HELLO") {
            sendMessage("HELLO");
        } else if (command == "LIST") {
            sendMessage("LIST");
        } else if (command.rfind("QUEUE", 0) == 0) {
            sendMessage("QUEUE");
        } else if (command.rfind("MOVE ", 0) == 0) {
            std::string position = command.substr(5);  // Extract position after "MOVE "
            sendMessage("MOVE~" + position);  // Send the MOVE command with the position
        } else {
            std::cout << "ERROR: Unknown command\n";
            continue;
        }

        receiveMessage();
    }

    close(sockfd);  // Close the socket when done
}

void Client::connectToServer() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "ERROR: Unable to create socket\n";
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        std::cerr << "ERROR: Connection failed\n";
        return;
    }

    std::cout << "Connected to server at " << serverIP << ":" << serverPort << "\n";
}

void Client::sendMessage(const std::string& message) {
    if (send(sockfd, message.c_str(), message.length(), 0) == -1) {
        std::cerr << "ERROR: Failed to send message\n";
    }
}

void Client::receiveMessage() {
    char buffer[1024] = {0};
    ssize_t bytesRead;
    std::string fullMessage;

    // Continuously receive data until no more data is left
    while ((bytesRead = recv(sockfd, buffer, sizeof(buffer) - 1, 0)) > 0) {
        buffer[bytesRead] = '\0'; // Null-terminate the received data
        fullMessage += buffer;
    }

    if (bytesRead == -1) {
        std::cerr << "ERROR: Failed to receive message\n";
        return;
    }

    std::cout << "Server: " << fullMessage << "\n";

    // Handle specific responses from the server
    if (fullMessage.find("ERROR") != std::string::npos) {
        std::cout << "ERROR: Server returned an error message\n";
    } else if (fullMessage == "LOGIN") {
        std::cout << "Login successful!\n";
    } else if (fullMessage == "HELLO") {
        std::cout << "Server says: HELLO\n";
    } else if (fullMessage.rfind("LIST", 0) == 0) {
        std::cout << "Server Player List: " << fullMessage.substr(5) << "\n";
    } else if (fullMessage.rfind("MOVE", 0) == 0) {
        std::cout << "Move confirmed: " << fullMessage.substr(5) << "\n";
    } else if (fullMessage.rfind("GAMEOVER", 0) == 0) {
        std::cout << "Game Over: " << fullMessage.substr(9) << "\n";
    }
}

int main() {
    Client client("127.0.0.1", 12345); // Replace with server IP & port
    client.start();
    return 0;
}
