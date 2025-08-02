#include "uci/command_parser.h"
#include <sstream>

namespace Cerberus {

namespace CommandParser {

void parse_command(const std::string& command, std::vector<std::string>& tokens) {
    // TODO: Implement command parsing
    std::istringstream iss(command);
    std::string token;
    while (iss >> token) {
        tokens.push_back(token);
    }
}

} // namespace CommandParser

} // namespace Cerberus 