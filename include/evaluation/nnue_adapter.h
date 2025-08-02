#pragma once

#include "core/board.h"
#include "core/types.h"

// Forward declare Stockfish classes to avoid including the full headers here
namespace Stockfish {
    class Position;
    namespace Eval::NNUE {
        class NetworkBig;
        class NetworkSmall;
        struct Networks;
    }
}

namespace Cerberus {
namespace NNUE_Adapter {

    // Initializes the NNUE network
    bool init(const std::string& model_path);

    // Evaluates the position using the NNUE
    Score evaluate(const Board& board);

} // namespace NNUE_Adapter
} // namespace Cerberus
