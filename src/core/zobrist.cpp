#include "core/zobrist.h"
#include "core/position.h"
#include <random>

namespace Cerberus {

// Zobrist keys
uint64_t Zobrist::piece_keys[PIECE_NB][SQUARE_NB];
uint64_t Zobrist::side_key_value;
uint64_t Zobrist::castling_keys[CASTLING_RIGHT_NB];
uint64_t Zobrist::en_passant_keys[SQUARE_NB];

void Zobrist::init() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    
    // Initialize piece keys
    for (Piece pc = NO_PIECE; pc < PIECE_NB; ++pc) {
        for (Square s = SQ_A1; s <= SQ_H8; ++s) {
            piece_keys[pc][s] = dis(gen);
        }
    }
    
    // Initialize side key
    side_key_value = dis(gen);
    
    // Initialize castling keys
    for (int i = 0; i < CASTLING_RIGHT_NB; ++i) {
        castling_keys[i] = dis(gen);
    }
    
    // Initialize en passant keys
    for (Square s = SQ_A1; s <= SQ_H8; ++s) {
        en_passant_keys[s] = dis(gen);
    }
}

Key Zobrist::piece_key(Piece pc, Square s) {
    return piece_keys[pc][s];
}

Key Zobrist::side_key() {
    return side_key_value;
}

Key Zobrist::castling_key(CastlingRights cr) {
    return castling_keys[cr];
}

Key Zobrist::en_passant_key(Square s) {
    return en_passant_keys[s];
}

Key Zobrist::compute_key(const Position& pos) {
    Key key = 0;
    
    // XOR in piece positions
    for (Square s = SQ_A1; s <= SQ_H8; ++s) {
        Piece pc = pos.piece_on(s);
        if (pc != NO_PIECE) {
            key ^= piece_key(pc, s);
        }
    }
    
    // XOR in side to move
    if (pos.side_to_move() == BLACK) {
        key ^= side_key();
    }
    
    // XOR in castling rights
    for (Color c = WHITE; c <= BLACK; ++c) {
        CastlingRights cr = pos.castling_rights(c);
        if (cr != NO_CASTLING) {
            key ^= castling_key(cr);
        }
    }
    
    // XOR in en passant square
    Square ep = pos.ep_square();
    if (ep != SQ_NONE) {
        key ^= en_passant_key(ep);
    }
    
    return key;
}

uint64_t Zobrist::random_uint64() {
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_int_distribution<uint64_t> dis;
    return dis(gen);
}

} // namespace Cerberus
