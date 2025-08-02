#include "evaluation/material.h"

namespace Cerberus {

namespace Material {

// Piece values (centipawns)
const int PIECE_VALUES[6] = {100, 320, 330, 500, 900, 20000}; // P, N, B, R, Q, K

// Material evaluation
Score evaluate_material_balance(const Board& board) {
    Score score = 0;
    
    // Count material for each side
    for (int square = 0; square < 64; square++) {
        Piece piece = board.get_piece(square);
        if (piece != EMPTY) {
            Color color = get_color(piece);
            PieceType type = get_piece_type(piece);
            
            Score piece_value = PIECE_VALUES[static_cast<int>(type) - 1];
            if (color == BLACK) {
                piece_value = -piece_value;
            }
            score += piece_value;
        }
    }
    
    return score;
}

} // namespace Material

} // namespace Cerberus 