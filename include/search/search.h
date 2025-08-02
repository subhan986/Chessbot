#pragma once

namespace Cerberus { class Position; }

#include "../core/move.h"
#include "../core/types.h"
#include "transposition_table.h"
#include <atomic>
#include <chrono>
#include <vector>

namespace Cerberus {

struct SearchLimits {
    int depth = 0;
    int movetime = 0;
};

struct SearchInfo {
    int depth;
    int seldepth;
    long long time;
    long long nodes;
    long long nps;
    Move best_move;
    std::vector<Move> pv;
    Score score;
    bool mate;
};

// Stack struct inspired by Stockfish to hold search-related data per ply.
struct Stack {
    Move* pv;
    int ply;
    Move currentMove;
    Move excludedMove;
    Score staticEval;
    int statScore;
    int moveCount;
    bool inCheck;
    bool ttPv;
    bool ttHit;
    Move killers[2];
};

class Search {
public:
    Search();
    ~Search();

    // Main search entry point
    static void iterative_deepening(const Position& pos, const SearchLimits& limits, SearchInfo* info);
    Move find_best_move(Board& board, SearchInfo& info);

    // Search control
    void stop() { stop_flag = true; }
    void set_time_limit(int ms) { time_limit = ms; }
    void set_max_depth(int depth) { max_depth = depth; }
    void set_max_nodes(long long nodes) { max_nodes = nodes; }

private:
    // Search control flags and limits
    std::atomic<bool> stop_flag;
    std::chrono::steady_clock::time_point start_time;
    int time_limit;
    int max_depth;
    long long max_nodes;

    // Search statistics
    long long nodes_searched;
    int current_depth;
    Move current_best_move;
    Score current_best_move_score;

    // Search parameters from UCI
    bool use_aspiration_windows;
    bool use_lmr;
    bool use_null_move_pruning;
    bool use_futility_pruning;
    bool use_see_pruning;

    TranspositionTable* tt;
    int history[2][64][64]; // [color][from][to]

    // Core search functions
    int lmr_reduction(int depth, int move_count);
    Score search(Board& board, Stack* ss, int depth, Score alpha, Score beta, bool is_pv);
    Score quiescence(Board& board, Stack* ss, Score alpha, Score beta);
    Score see(Board& board, Move move);

    // Time and node limit checks
    bool should_stop() const;
    bool is_time_up() const;
    bool is_node_limit_reached() const;

    void update_search_info(SearchInfo& info);
};

} // namespace Cerberus
