#pragma once

#include "../core/board.h"
#include "../core/move.h"
#include "../search/search.h"
#include "../search/thread.h"
#include <string>
#include <map>

namespace Cerberus {

class UCIInterface {
private:
    Board board;
    Search search;
    Thread search_thread;
    
    // UCI options
    std::map<std::string, std::string> options;
    
    // Search parameters
    int hash_size;
    int threads;
    int multi_pv;
    int move_overhead;
    int min_thinking_time;
    int slow_mover;
    
    // Analysis mode
    bool analyse_mode;
    bool limit_strength;
    int uci_elo;
    
    // Syzygy tablebases
    std::string syzygy_path;
    int syzygy_probe_depth;
    bool syzygy_50_move_rule;
    int syzygy_probe_limit;

public:
    UCIInterface();
    
    // Main UCI loop
    void loop();
    
    // UCI commands
    void handleUCI();
    void handleIsReady();
    void handleSetOption(const std::string& command);
    void handleUCINewGame();
    void handlePosition(const std::string& command);
    void handleGo(const std::string& command);
    void handleStop();
    void handleQuit();
    void handleDebug(const std::string& command);
    void handleRegister(const std::string& command);
    void handleInfo(const std::string& command);
    
    // Search control
    void startSearch(const std::string& command);
    void stopSearch();
    
    // Time management
    int calculateTimeLimit(const std::string& command);
    int calculateMoveTime(const std::string& command);
    
    // Move parsing
    Move parseMove(const std::string& str);
    std::string moveToString(Move move);
    
    // Information output
    void sendUCIInfo(const SearchInfo& info);
    void sendBestMove(Move move);
    void sendInfo(const std::string& info);
    
private:
    // Helper functions
    void parsePosition(const std::string& fen, const std::string& moves);
    void parseGoCommand(const std::string& command);
    void setOption(const std::string& name, const std::string& value);
    
    // Default options
    void setDefaultOptions();
    
    // Search parameters parsing
    int parseIntOption(const std::string& value, int min, int max, int default_val);
    bool parseBoolOption(const std::string& value, bool default_val);
};

} // namespace Cerberus
