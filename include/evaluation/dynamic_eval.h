#pragma once

#include "../core/board.h"
#include "../core/types.h"

namespace Cerberus {

namespace DynamicEval {

// Main evaluation function
Score evaluate(const Board& board);

// Game phase detection
GamePhase get_game_phase(const Board& board);

// Material evaluation
Score evaluate_material(const Board& board);

// Positional evaluation
Score evaluate_positional(const Board& board);

// Pawn structure evaluation
Score evaluate_pawn_structure(const Board& board);

// Mobility evaluation
Score evaluate_mobility(const Board& board);

// King safety evaluation
Score evaluate_king_safety(const Board& board);

} // namespace DynamicEval

} // namespace Cerberus 