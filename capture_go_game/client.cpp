#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

using namespace std;

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 7777

mutex mtx;
string currentTurn = ""; // Track whose turn it is

void showInstructions() {
    cout << "\n📜  CAPTURE GO - COMMANDS LIST  📜\n";
    cout << "---------------------------------\n";
    cout << "1️⃣  LIST     - Show all online players\n";
    cout << "2️⃣  QUEUE    - Join the queue for a match\n";
    cout << "3️⃣  MOVE X   - Make a move (X is the board position)\n";
    cout << "4️⃣  EXIT     - Disconnect from the server\n";
    cout << "5️⃣  HELP     - Show this menu again\n";
    cout << "---------------------------------\n\n";
}

void receiveMessages(int sock) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, 1024);
        if (valread <= 0) {
            cout << "Disconnected from server." << endl;
            close(sock);
            exit(0);
        }

        string message(buffer);
        mtx.lock();

        // Handle special messages
        if (message.rfind("TURN~", 0) == 0) { // "TURN~<player>"
            currentTurn = message.substr(5);
            cout << "\n🎯 It's " << (currentTurn == "YOU" ? "your" : currentTurn + "'s") << " turn!\n";
            if (currentTurn == "YOU") {
                cout << "🔹 Type MOVE <position> to make a move.\n";
            }
        }
        else if (message.rfind("BOARD~", 0) == 0) { // "BOARD~..."
            cout << "\n📢 Updated Board:\n";
            cout << message.substr(6) << "\n";
            cout << "\n⌛ Waiting for the next update...\n";
        }
        else if (message == "GAMEOVER") { // Game Over Signal
            cout << "\n🏆 GAME OVER! Thank you for playing!\n";
        }
        else {
            cout << message << endl;
        }

        mtx.unlock();
    }
}

void sendCommand(int sock, const string& command) {
    string formattedCommand = command + "\n";
    send(sock, formattedCommand.c_str(), formattedCommand.length(), 0);
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Socket creation error" << endl;
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address? Address not supported" << endl;
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed" << endl;
        return -1;
    }

    thread recvThread(receiveMessages, sock);
    recvThread.detach();

    cout << "\n✅ Connected to the server!\n";
    showInstructions(); // Show commands after connecting

    string input;
    while (true) {
        cout << "\n🔹 Enter Command: ";
        getline(cin, input);

        if (input == "EXIT") {
            cout << "🔴 Disconnecting..." << endl;
            close(sock);
            break;
        }
        else if (input == "HELP") {
            showInstructions();
            continue;
        }

        sendCommand(sock, input);
    }

    return 0;
}
