#include "search/move_ordering.h"
#include "core/move_generator.h"
#include <algorithm>

namespace Cerberus {

MovePicker::MovePicker(const Board& b, Move tt_move, Move killer1, Move killer2, const int (*history)[64][64])
    : board(b), current_index(0), history(history) {
    MoveGenerator::generate_moves(board, moves);
    score_moves(tt_move, killer1, killer2);
    std::sort(moves.begin(), moves.end(), [](const Move& a, const Move& b) {
        return a.get_score() > b.get_score();
    });
}

Move MovePicker::next_move() {
    if (current_index < moves.size()) {
        return moves[current_index++];
    }
    return Move::none();
}

void MovePicker::score_moves(Move tt_move, Move killer1, Move killer2) {
    for (Move& move : moves) {
        move.set_score(get_move_score(move, tt_move, killer1, killer2));
    }
}

Score MovePicker::get_move_score(Move move, Move tt_move, Move killer1, Move killer2) {
    if (move == tt_move) {
        return 100000;
    }
    if (move == killer1) {
        return 90000;
    }
    if (move == killer2) {
        return 89000;
    }
    if (board.is_capture(move)) {
        // MVV-LVA: Most Valuable Victim - Least Valuable Attacker
        return 80000 + MoveOrdering::get_piece_value(board.piece_on(move.to_sq())) - MoveOrdering::get_piece_value(board.piece_on(move.from_sq()));
    } else {
        return history[board.get_side_to_move()][move.from_sq()][move.to_sq()];
    }
}

namespace MoveOrdering {

void order_moves(const Board& board, std::vector<Move>& moves) {
    // This function is now a wrapper around the MovePicker class.
    // For simplicity, we'll just sort the moves based on a simple score.
    std::sort(moves.begin(), moves.end(), [&board](const Move& a, const Move& b) {
        return get_move_score(board, a) > get_move_score(board, b);
    });
}

Score get_move_score(const Board& board, Move move) {
    if (board.is_capture(move)) {
        return get_piece_value(board.piece_on(move.to_sq())) - get_piece_value(board.piece_on(move.from_sq()));
    }
    return 0;
}

Score get_piece_value(Piece piece) {
    switch (type_of(piece)) {
        case PAWN: return 100;
        case KNIGHT: return 320;
        case BISHOP: return 330;
        case ROOK: return 500;
        case QUEEN: return 900;
        case KING: return 20000;
        default: return 0;
    }
}

} // namespace MoveOrdering
} // namespace Cerberus
