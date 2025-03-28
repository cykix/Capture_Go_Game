#include "protocol.hpp"
#include <sstream>

namespace Protocol {

std::string formatMessage(const std::string& command, const std::vector<std::string>& args) {
    std::ostringstream message;
    message << command;

    for (const std::string& arg : args) {
        message << "~" << arg;
    }

    return message.str();
}

bool parseMessage(const std::string& message, std::string& command, std::vector<std::string>& args) {
    std::istringstream stream(message);
    std::string token;

    // Check for empty message or invalid format
    if (!std::getline(stream, command, '~') || command.empty()) {
        return false; // Invalid message format or empty command
    }

    while (std::getline(stream, token, '~')) {
        // Optionally trim spaces around the token if needed
        args.push_back(token);
    }

    return true;
}

} // namespace Protocol
