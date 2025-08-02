#include "evaluation/dynamic_eval.h"
#include "evaluation/material.h"
#include "evaluation/positional.h"
#include "evaluation/pawn_structure.h"

namespace Cerberus {

namespace DynamicEval {

// Main evaluation function
Score evaluate(const Board& board) {
    Score score = 0;
    
    // Material evaluation
    score += evaluate_material(board);
    
    // Positional evaluation
    score += evaluate_positional(board);
    
    // Pawn structure evaluation
    score += evaluate_pawn_structure(board);
    
    // Mobility evaluation (simplified)
    // score += evaluate_mobility(board);
    
    // King safety evaluation (simplified)
    // score += evaluate_king_safety(board);
    
    return score;
}

// Game phase detection
GamePhase get_game_phase(const Board& board) {
    int piece_count = 0;
    
    // Count non-pawn pieces
    for (PieceType type = KNIGHT; type <= QUEEN; type = static_cast<PieceType>(type + 1)) {
        piece_count += board.get_piece_bitboard(WHITE, type).count();
        piece_count += board.get_piece_bitboard(BLACK, type).count();
    }
    
    if (piece_count <= 6) return ENDGAME;
    if (piece_count <= 12) return LATE_MIDDLEGAME;
    return MIDDLEGAME;
}

// Material evaluation
Score evaluate_material(const Board& board) {
    return Material::evaluate_material_balance(board);
}

// Positional evaluation
Score evaluate_positional(const Board& board) {
    return Positional::evaluate_positional_score(board);
}

// Pawn structure evaluation
Score evaluate_pawn_structure(const Board& board) {
    return PawnStructure::evaluate_pawn_structure(board);
}

// Mobility evaluation
Score evaluate_mobility(const Board& board) {
    // Simplified mobility evaluation
    return 0;
}

// King safety evaluation
Score evaluate_king_safety(const Board& board) {
    // Simplified king safety evaluation
    return 0;
}

} // namespace DynamicEval

} // namespace Cerberus 