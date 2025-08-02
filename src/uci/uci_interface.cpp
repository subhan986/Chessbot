#include "uci/uci_interface.h"
#include "core/position.h"
#include "search/search.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace Cerberus {

UCIInterface::UCIInterface() {
    setDefaultOptions();
}

void UCIInterface::loop() {
    std::string command;
    while (std::getline(std::cin, command)) {
        try {
            if (command == "uci") {
                handleUCI();
            } else if (command == "isready") {
                handleIsReady();
            } else if (command.substr(0, 8) == "setoption") {
                handleSetOption(command);
            } else if (command == "ucinewgame") {
                handleUCINewGame();
            } else if (command.substr(0, 8) == "position") {
                handlePosition(command);
            } else if (command.substr(0, 2) == "go") {
                handleGo(command);
            } else if (command == "stop") {
                handleStop();
            } else if (command == "quit") {
                handleQuit();
                break;
            }
        } catch (const std::exception& e) {
            std::cerr << "Error processing command: " << e.what() << std::endl;
        }
    }
}

void UCIInterface::handleUCI() {
    std::cout << "id name Cerberus" << std::endl;
    std::cout << "id author Cerberus Team" << std::endl;
    
    // Send options
    for (const auto& option : options) {
        std::cout << "option name " << option.first << " type string default " << option.second << std::endl;
    }
    
    std::cout << "uciok" << std::endl;
}

void UCIInterface::handleIsReady() {
    std::cout << "readyok" << std::endl;
}

void UCIInterface::handleSetOption(const std::string& command) {
    std::istringstream iss(command);
    std::string token;
    std::string name;
    std::string value;

    iss >> token; // "setoption"
    iss >> token; // "name"
    
    // Read the option name
    while (iss >> token && token != "value") {
        if (!name.empty()) name += " ";
        name += token;
    }

    // Read the option value
    while (iss >> token) {
        if (!value.empty()) value += " ";
        value += token;
    }

    if (options.count(name)) {
        options[name] = value;
    } else {
        std::cerr << "Unknown option: " << name << std::endl;
    }
}

void UCIInterface::handleUCINewGame() {
    // Reset position to starting position
    StateInfo si;
    board.set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false, &si);
}

void UCIInterface::handlePosition(const std::string& command) {
    std::istringstream iss(command);
    std::string token;
    iss >> token; // "position"
    
    iss >> token;
    if (token == "startpos") {
        StateInfo si;
        board.set("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", false, &si);
    } else if (token == "fen") {
        std::string fen;
        while (iss >> token && token != "moves") {
            fen += token + " ";
        }
        StateInfo si;
        board.set(fen, false, &si);
    }
    
    // Parse moves if any
    if (token == "moves") {
        while (iss >> token) {
            Move move = MoveUtils::parse_move(token);
            if (move.is_ok()) {
                StateInfo newSi;
                board.do_move(move, newSi);
            }
        }
    }
}

void UCIInterface::handleGo(const std::string& command) {
    // Parse go command parameters
    std::istringstream iss(command);
    std::string token;
    iss >> token; // "go"
    
    SearchLimits limits;
    
    while (iss >> token) {
        if (token == "depth") {
            iss >> limits.depth;
        } else if (token == "movetime") {
            iss >> limits.movetime;
        }
    }
    
    // Start search
    search_thread.start_searching(board.get_position(), limits);
}

void UCIInterface::handleStop() {
    search_thread.stop();
}

void UCIInterface::handleQuit() {
    stopSearch();
}

void UCIInterface::startSearch(const SearchInfo& info) {
    // This function is now handled by the Thread class
}

void UCIInterface::stopSearch() {
    // This function is now handled by the Thread class
}

void UCIInterface::sendBestMove(Move move) {
    std::cout << "bestmove " << MoveUtils::to_string(move) << std::endl;
}

void UCIInterface::setDefaultOptions() {
    options["Hash"] = "16";
    options["Threads"] = "1";
    options["MultiPV"] = "1";
}

} // namespace Cerberus
