#include "search/search.h"
#include "evaluation/evaluation.h"
#include "core/move_generator.h"
#include "search/move_ordering.h"
#include <algorithm>
#include <chrono>
#include <iostream>
#include <cmath>

namespace Cerberus {

Search::Search() : stop_flag(false), time_limit(0), max_depth(64), max_nodes(0),
                   nodes_searched(0), current_depth(0), current_best_move(Move::none()),
                   current_best_move_score(0), use_aspiration_windows(true), use_lmr(true),
                   use_null_move_pruning(true), use_futility_pruning(true), use_see_pruning(true) {
    tt = new TranspositionTable();
    for (int i = 0; i < 2; ++i) {
        for (int j = 0; j < 64; ++j) {
            for (int k = 0; k < 64; ++k) {
                history[i][j][k] = 0;
            }
        }
    }
}

Search::~Search() {
    delete tt;
}

void Search::iterative_deepening(const Position& pos, const SearchLimits& limits, SearchInfo* info) {
    Search search;
    search.max_depth = limits.depth;
    search.time_limit = limits.movetime;
    Board board;
    board.set(pos.get_fen(), false, nullptr);
    search.find_best_move(board, *info);
    std::cout << "bestmove " << MoveUtils::to_string(info->best_move) << std::endl;
}

Move Search::find_best_move(Board& board, SearchInfo& info) {
    start_time = std::chrono::steady_clock::now();
    nodes_searched = 0;
    current_depth = 0;
    stop_flag = false;

    Stack stack[MAX_PLY + 2];
    (stack + 1)->ply = 0;
    for (int i = 0; i < MAX_PLY + 2; ++i) {
        stack[i].killers[0] = Move::none();
        stack[i].killers[1] = Move::none();
    }

    Score alpha = -VALUE_INFINITE;
    Score beta = VALUE_INFINITE;
    Score delta = 16; // Initial aspiration window size

    for (int i = 1; i <= max_depth; ++i) {
        current_depth = i;
        Score score;

        if (use_aspiration_windows && i > 1) {
            alpha = current_best_move_score - delta;
            beta = current_best_move_score + delta;
        }

        while (true) {
            score = search(board, stack + 1, i, alpha, beta, true);

            if (should_stop()) {
                break;
            }

            if (score <= alpha) { // Fail low
                alpha = -VALUE_INFINITE;
            } else if (score >= beta) { // Fail high
                beta = VALUE_INFINITE;
            } else {
                break; // Success
            }
            delta *= 2; // Widen the window for the re-search
        }

        if (should_stop()) {
            break;
        }
        current_best_move_score = score;
    }

    update_search_info(info);
    return current_best_move;
}

Score Search::search(Board& board, Stack* ss, int depth, Score alpha, Score beta, bool is_pv) {
    if (should_stop()) {
        return 0;
    }

    nodes_searched++;
    ss->ply = (ss - 1)->ply + 1;

    if (depth <= 0) {
        return quiescence(board, ss, alpha, beta);
    }

    if (board.is_repetition() || board.is_fifty_move_draw()) {
        return 0;
    }

    if (ss->ply >= MAX_PLY) {
        return Evaluation::evaluate(board);
    }

    ss->inCheck = board.is_in_check();
    if (ss->inCheck) {
        depth++;
    }

    // Transposition Table Probe
    TTEntry* tt_entry = tt->probe(board.get_zobrist_key());
    if (tt_entry != nullptr && tt_entry->depth >= depth) {
        if (tt_entry->flag == TTEntry::Flag::EXACT) {
            return tt_entry->score;
        }
        if (tt_entry->flag == TTEntry::Flag::LOWER_BOUND && tt_entry->score >= beta) {
            return tt_entry->score;
        }
        if (tt_entry->flag == TTEntry::Flag::UPPER_BOUND && tt_entry->score <= alpha) {
            return tt_entry->score;
        }
    }

    if (use_null_move_pruning && !is_pv && !ss->inCheck && depth >= 3) {
        board.make_null_move();
        Score null_score = -search(board, ss + 1, depth - 3, -beta, -beta + 1, false);
        board.unmake_null_move();
        if (null_score >= beta) {
            return beta;
        }
    }

    Move tt_move = tt_entry ? tt_entry->move : Move::none();
    MovePicker move_picker(board, tt_move, (ss - 1)->killers[0], (ss - 1)->killers[1], history);
    Move move;
    Score static_eval = Evaluation::evaluate(board);
    ss->staticEval = static_eval;

    if (use_futility_pruning && depth < 7 && !is_pv && !ss->inCheck) {
        if (static_eval - 100 * depth > beta) {
            return beta;
        }
    }
    Move best_move = Move::none();
    Score best_score = -VALUE_INFINITE;
    int move_count = 0;
    TTEntry::Flag tt_flag = TTEntry::Flag::UPPER_BOUND;

    while ((move = move_picker.next_move()) != Move::none()) {
        move_count++;

        if (use_see_pruning && board.is_capture(move) && see(board, move) < 0) {
            continue;
        }

        if (use_futility_pruning && depth < 7 && !is_pv && !ss->inCheck && !board.is_capture(move) && !board.is_promotion(move)) {
            if (static_eval + 100 < alpha) {
                continue;
            }
        }

        board.make_move(move);
        Score score;
        if (move_count == 1) {
            score = -search(board, ss + 1, depth - 1, -beta, -alpha, true);
        } else {
            int reduction = lmr_reduction(depth, move_count);
            score = -search(board, ss + 1, depth - 1 - reduction, -alpha - 1, -alpha, false);
            if (reduction > 0 && score > alpha) {
                score = -search(board, ss + 1, depth - 1, -alpha - 1, -alpha, false);
            }
            if (score > alpha && score < beta) {
                score = -search(board, ss + 1, depth - 1, -beta, -alpha, true);
            }
        }
        board.unmake_move();

        if (should_stop()) {
            return 0;
        }

        if (score > best_score) {
            best_score = score;
            if (score > alpha) {
                alpha = score;
                best_move = move;
                tt_flag = TTEntry::Flag::EXACT;
                if (ss->ply == 1) {
                    current_best_move = move;
                }
                if (alpha >= beta) {
                    tt_flag = TTEntry::Flag::LOWER_BOUND;
                    if (!board.is_capture(move)) {
                        history[board.get_side_to_move()][move.from_sq()][move.to_sq()] += depth * depth;
                        if (move != ss->killers[0]) {
                            ss->killers[1] = ss->killers[0];
                            ss->killers[0] = move;
                        }
                    }
                    break; // Beta cutoff
                }
            }
        }
    }

    if (move_count == 0) {
        if (ss->inCheck) {
            return mated_in(ss->ply);
        }
        return 0; // Stalemate
    }

    tt->store(board.get_zobrist_key(), best_move, best_score, depth, tt_flag, ss->ply);
    return best_score;
}

Score Search::see(Board& board, Move move) {
    Square from = move.from_sq();
    Square to = move.to_sq();
    PieceType captured = board.get_piece_type(to);
    Score gain = Evaluation::piece_value[captured];
    board.make_move(move);
    gain -= see_recursive(board, to);
    board.unmake_move();
    return gain;
}

Score Search::see_recursive(Board& board, Square to) {
    Move smallest_attacker = board.get_smallest_attacker(to);
    if (smallest_attacker == Move::none()) {
        return 0;
    }
    PieceType captured = board.get_piece_type(to);
    Score gain = Evaluation::piece_value[captured];
    board.make_move(smallest_attacker);
    gain -= see_recursive(board, to);
    board.unmake_move();
    return gain;
}

int Search::lmr_reduction(int depth, int move_count) {
    if (!use_lmr) return 0;
    return static_cast<int>(0.5 + log(depth) * log(move_count) / 2.0);
}

Score Search::quiescence(Board& board, Stack* ss, Score alpha, Score beta) {
    if (should_stop()) {
        return 0;
    }

    nodes_searched++;
    ss->ply = (ss - 1)->ply + 1;

    // Transposition Table Probe
    TTEntry* tt_entry = tt->probe(board.get_zobrist_key());
    if (tt_entry != nullptr) {
        if (tt_entry->flag == TTEntry::Flag::EXACT) {
            return tt_entry->score;
        }
        if (tt_entry->flag == TTEntry::Flag::LOWER_BOUND && tt_entry->score >= beta) {
            return tt_entry->score;
        }
        if (tt_entry->flag == TTEntry::Flag::UPPER_BOUND && tt_entry->score <= alpha) {
            return tt_entry->score;
        }
    }

    Score stand_pat = Evaluation::evaluate(board);

    if (stand_pat >= beta) {
        return beta;
    }

    if (alpha < stand_pat) {
        alpha = stand_pat;
    }

    MovePicker move_picker(board, Move::none(), Move::none(), Move::none());
    Move move;
    TTEntry::Flag tt_flag = TTEntry::Flag::UPPER_BOUND;

    while ((move = move_picker.next_move()) != Move::none()) {
        if (!board.is_capture(move)) continue;

        board.make_move(move);
        Score score = -quiescence(board, ss + 1, -beta, -alpha);
        board.unmake_move();

        if (should_stop()) {
            return 0;
        }

        if (score >= beta) {
            tt->store(board.get_zobrist_key(), Move::none(), beta, 0, TTEntry::Flag::LOWER_BOUND, ss->ply);
            return beta;
        }
        if (score > alpha) {
            alpha = score;
            tt_flag = TTEntry::Flag::EXACT;
        }
    }

    tt->store(board.get_zobrist_key(), Move::none(), alpha, 0, tt_flag, ss->ply);
    return alpha;
}

bool Search::is_time_up() const {
    if (time_limit == 0) return false;
    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
    return elapsed.count() >= time_limit;
}

bool Search::is_node_limit_reached() const {
    if (max_nodes == 0) return false;
    return nodes_searched >= max_nodes;
}

bool Search::should_stop() const {
    return stop_flag || is_time_up() || is_node_limit_reached();
}

void Search::update_search_info(SearchInfo& info) {
    info.depth = current_depth;
    info.seldepth = 0; // Simplified for now
    info.nodes = nodes_searched;
    info.best_move = current_best_move;
    info.score = 0; // Simplified for now
    info.mate = false; // Simplified for now

    auto now = std::chrono::steady_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start_time);
    info.time = elapsed.count();

    if (info.time > 0) {
        info.nps = (nodes_searched * 1000) / info.time;
    } else {
        info.nps = 0;
    }
}

} // namespace Cerberus
