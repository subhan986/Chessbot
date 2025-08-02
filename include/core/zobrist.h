#pragma once

#include "types.h"
#include <cstdint>

namespace Cerberus {

// Forward declaration
class Position;

class Zobrist {
public:
    static void init();
    
    // Zobrist key generation
    static Key piece_key(Piece pc, Square s);
    static Key side_key();
    static Key castling_key(CastlingRights cr);
    static Key en_passant_key(Square s);
    
    // Compute full position key
    static Key compute_key(const Position& pos);

private:
    // Zobrist keys
    static uint64_t piece_keys[PIECE_NB][SQUARE_NB];
    static uint64_t side_key_value;
    static uint64_t castling_keys[CASTLING_RIGHT_NB];
    static uint64_t en_passant_keys[SQUARE_NB];
    
    // Random number generation
    static uint64_t random_uint64();
};

} // namespace Cerberus
