#pragma once

#include "../core/board.h"
#include "../core/types.h"

namespace Cerberus {

namespace Material {

// Material evaluation
Score evaluate_material_balance(const Board& board);

// Piece values
extern const int PIECE_VALUES[6];

} // namespace Material

} // namespace Cerberus 