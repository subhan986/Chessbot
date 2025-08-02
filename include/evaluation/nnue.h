#pragma once

#include "../core/board.h"
#include "../core/types.h"

namespace Cerberus {

namespace NNUE {

// NNUE evaluation
Score evaluate(const Board& board);

} // namespace NNUE

} // namespace Cerberus 