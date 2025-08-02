#pragma once

#include "../core/board.h"
#include "../core/types.h"

namespace Cerberus {

namespace Positional {

// Positional evaluation
Score evaluate_positional_score(const Board& board);
Score evaluate_piece_position(const Board& board, Color color);

// Positional tables
extern const int PAWN_POSITION_TABLE[64];
extern const int KNIGHT_POSITION_TABLE[64];
extern const int BISHOP_POSITION_TABLE[64];
extern const int ROOK_POSITION_TABLE[64];
extern const int QUEEN_POSITION_TABLE[64];
extern const int KING_POSITION_TABLE[64];

// Endgame detection
bool is_endgame(const Board& board);

} // namespace Positional

} // namespace Cerberus 