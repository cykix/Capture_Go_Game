#include "server.h"

Server::Server() {
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == 0) {
        cerr << "Socket creation failed!" << endl;
        exit(EXIT_FAILURE);
    }

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(DEFAULT_PORT);

    if (bind(serverSocket, (struct sockaddr*)&address, sizeof(address)) < 0) {
        cerr << "Bind failed!" << endl;
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, MAX_CLIENTS) < 0) {
        cerr << "Listen failed!" << endl;
        exit(EXIT_FAILURE);
    }
}

void Server::start() {
    cout << "Server started on port " << DEFAULT_PORT << endl;

    while (true) {
        sockaddr_in clientAddr{};
        socklen_t addrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen);

        if (clientSocket < 0) {
            cerr << "Accept failed!" << endl;
            continue;
        }

        thread(&Server::handleClient, this, clientSocket).detach();
    }
}

void Server::handleClient(int clientSocket) {
    char buffer[1024];
    string username;

    sendCommand(clientSocket, "HELLO~Server~CAPTUREGO");

    // Login handling
    sendCommand(clientSocket, "LOGIN~Enter your username: ");
    int valread = read(clientSocket, buffer, 1024);
    username = string(buffer, valread);
    username.erase(username.find_last_not_of(" \n\r\t") + 1);  // Trim whitespace

    mtx.lock();
    while (usernames.find(username) != usernames.end()) {
        sendCommand(clientSocket, "ALREADYLOGGEDIN");
        valread = read(clientSocket, buffer, 1024);
        username = string(buffer, valread);
        username.erase(username.find_last_not_of(" \n\r\t") + 1);
    }

    usernames.insert(username);
    clientSockets[clientSocket] = username;
    mtx.unlock();

    sendCommand(clientSocket, "LOGIN");

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        valread = read(clientSocket, buffer, 1024);
        if (valread <= 0) break;

        string command(buffer, valread);
        command.erase(command.find_last_not_of(" \n\r\t") + 1);
        processCommand(clientSocket, command);
    }

    // Cleanup on disconnect
    mtx.lock();
    usernames.erase(username);
    clientSockets.erase(clientSocket);
    auto it = find(queue.begin(), queue.end(), clientSocket);
    if (it != queue.end()) {
        queue.erase(it);
    }

    close(clientSocket);
    mtx.unlock();

    cout << "Client " << username << " disconnected!" << endl;
}

void Server::sendCommand(int clientSocket, const string& command) {
    string formattedCommand = command + "\n";
    send(clientSocket, formattedCommand.c_str(), formattedCommand.length(), 0);
}

void Server::processCommand(int clientSocket, const string& command) {
    if (command == "LIST") {
        string userList = "LIST";
        mtx.lock();
        for (const auto& user : usernames) {
            userList += "~" + user;
        }
        mtx.unlock();
        sendCommand(clientSocket, userList);
    }
    else if (command == "QUEUE") {
        mtx.lock();
        queue.push_back(clientSocket);
        if (queue.size() == 2) {
            startGame();
        }
        mtx.unlock();
    }
    else if (command.rfind("MOVE ", 0) == 0 || command.rfind("MOVE~", 0) == 0) {
        string moveValue = command.substr(command.find(' ') + 1); // Extract move after space
        processMove(clientSocket, moveValue);
    }

    else {
        sendCommand(clientSocket, "ERROR~Unknown command");
    }
}

void Server::startGame() {
    if (queue.size() < 2) return;

    int player1 = queue[0];
    int player2 = queue[1];

    // Assign colors
    sendCommand(player1, "NEWGAME~You are BLACK (B)");
    sendCommand(player2, "NEWGAME~You are WHITE (W)");

    // Set initial turn to BLACK (first player)
    currentTurn = player1;

    queue.clear();

    sendBoardState(player1);
    sendBoardState(player2);

    sendCommand(player1, "Your Turn! Type MOVE <position> to make a move.");
    sendCommand(player2, "Waiting for the other player...");
}


void Server::processMove(int clientSocket, const string& move) {
    int row, col;
    if (sscanf(move.c_str(), "%d", &row) != 1 || row < 0 || row > 48) {
        sendCommand(clientSocket, "ERROR~Invalid move");
        return;
    }

    int colIndex = row % 7;
    int rowIndex = row / 7;

    mtx.lock();

    // Ensure it's the correct player's turn
    if (clientSocket != currentTurn) {
        sendCommand(clientSocket, "ERROR~Not your turn!");
        mtx.unlock();
        return;
    }

    char currentPlayer = (clientSocket == queue[0]) ? 'B' : 'W';

    if (gameBoard.isValidMove(rowIndex, colIndex)) {
        gameBoard.makeMove(rowIndex, colIndex, currentPlayer);
        gameBoard.removeCapturedStones(currentPlayer);

        // Swap turn
        currentTurn = (clientSocket == queue[0]) ? queue[1] : queue[0];

        // Send updated board to both players
        sendBoardState(queue[0]);
        sendBoardState(queue[1]);

        // Notify turns
        sendCommand(currentTurn, "Your Turn! Type MOVE <position> to make a move.");
        sendCommand(clientSocket, "Waiting for the other player...");
    }
    else {
        sendCommand(clientSocket, "ERROR~Invalid move");
    }

    mtx.unlock();
}



void Server::gameOver(const string& reason, const string& winner) {
    string gameOverMessage = "*GAMEOVER*" + reason;
    if (!winner.empty()) {
        gameOverMessage += "~" + winner;
    }

    for (auto& client : clientSockets) {
        sendCommand(client.first, gameOverMessage);
    }
}

void Server::sendBoardState(int clientSocket) {
    string boardState = "BOARD\n";
    boardState += "   0 1 2 3 4 5 6\n";  // Column numbers
    boardState += "  ----------------\n";

    for (int i = 0; i < Board::SIZE; i++) {
        boardState += to_string(i) + " | ";  // Row number
        for (int j = 0; j < Board::SIZE; j++) {
            string stone = (gameBoard.grid[i][j] == '.' ? "·" : string(1, gameBoard.grid[i][j]));
            boardState += stone + " ";
        }
        boardState += "\n";
    }

    sendCommand(clientSocket, boardState);
}







int main() {
    Server server;
    server.start();
    return 0;
}

