#pragma once

#include "../core/board.h"
#include "../core/types.h"

namespace Cerberus {

namespace PawnStructure {

// Pawn structure evaluation
Score evaluate_pawn_structure(const Board& board);

// Pawn features
bool is_isolated_pawn(const Board& board, int square, Color color);
bool is_doubled_pawn(const Board& board, int square, Color color);
bool is_backward_pawn(const Board& board, int square, Color color);
bool is_passed_pawn(const Board& board, int square, Color color);

} // namespace PawnStructure

} // namespace Cerberus 