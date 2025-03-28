#ifndef PROTOCOL_HPP
#define PROTOCOL_HPP

#include <string>
#include <vector>

namespace Protocol {
    // Client-side commands
    const std::string HELLO = "HELLO";
    const std::string LOGIN = "LOGIN";
    const std::string LIST = "LIST";
    const std::string QUEUE = "QUEUE";
    const std::string MOVE = "MOVE";

    // Server-side commands
    const std::string HELLO_SERVER = "HELLO";
    const std::string LOGIN_SERVER = "LOGIN";
    const std::string ALREADYLOGGEDIN = "ALREADYLOGGEDIN";
    const std::string LIST_SERVER = "LIST";
    const std::string NEWGAME = "NEWGAME";
    const std::string MOVE_SERVER = "MOVE";
    const std::string GAMEOVER = "GAMEOVER";
    const std::string ERROR = "ERROR";

    // Function to format a message with arguments (separated by '~')
    std::string formatMessage(const std::string& command, const std::vector<std::string>& args = {});

    // Function to parse a message into command and arguments
    bool parseMessage(const std::string& message, std::string& command, std::vector<std::string>& args);
}

#endif // PROTOCOL_HPP
