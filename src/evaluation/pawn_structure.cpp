#include "evaluation/pawn_structure.h"

namespace Cerberus {

namespace PawnStructure {

// Pawn structure evaluation
Score evaluate_pawn_structure(const Board& board) {
    Score score = 0;
    
    // Evaluate pawn structure for both colors
    score += evaluate_pawn_structure_color(board, WHITE);
    score -= evaluate_pawn_structure_color(board, BLACK);
    
    return score;
}

// Evaluate pawn structure for a specific color
Score evaluate_pawn_structure_color(const Board& board, Color color) {
    Score score = 0;
    Bitboard pawns = board.get_piece_bitboard(color, PAWN);
    
    while (pawns) {
        int square = pawns.pop_lsb();
        
        // Penalize isolated pawns
        if (is_isolated_pawn(board, square, color)) {
            score -= 10;
        }
        
        // Penalize doubled pawns
        if (is_doubled_pawn(board, square, color)) {
            score -= 5;
        }
        
        // Bonus for passed pawns
        if (is_passed_pawn(board, square, color)) {
            score += 20;
        }
        
        // Penalize backward pawns
        if (is_backward_pawn(board, square, color)) {
            score -= 5;
        }
    }
    
    return score;
}

// Check if a pawn is isolated
bool is_isolated_pawn(const Board& board, int square, Color color) {
    int file = get_file(square);
    Bitboard pawns = board.get_piece_bitboard(color, PAWN);
    
    // Check adjacent files for friendly pawns
    for (int adj_file = file - 1; adj_file <= file + 1; adj_file++) {
        if (adj_file >= 0 && adj_file < 8 && adj_file != file) {
            Bitboard file_pawns = pawns & get_file_bb(adj_file);
            if (file_pawns) {
                return false; // Found a friendly pawn on adjacent file
            }
        }
    }
    
    return true; // No friendly pawns on adjacent files
}

// Check if a pawn is doubled
bool is_doubled_pawn(const Board& board, int square, Color color) {
    int file = get_file(square);
    Bitboard pawns = board.get_piece_bitboard(color, PAWN);
    Bitboard file_pawns = pawns & get_file_bb(file);
    
    return file_pawns.count() > 1;
}

// Check if a pawn is backward
bool is_backward_pawn(const Board& board, int square, Color color) {
    // Simplified implementation
    return false;
}

// Check if a pawn is passed
bool is_passed_pawn(const Board& board, int square, Color color) {
    int file = get_file(square);
    int rank = get_rank(square);
    Color opponent = (color == WHITE) ? BLACK : WHITE;
    Bitboard opponent_pawns = board.get_piece_bitboard(opponent, PAWN);
    
    // Check if there are opponent pawns on the same file or adjacent files
    // that are ahead of this pawn
    for (int check_file = file - 1; check_file <= file + 1; check_file++) {
        if (check_file >= 0 && check_file < 8) {
            Bitboard file_pawns = opponent_pawns & get_file_bb(check_file);
            
            // Check if any opponent pawn is ahead
            if (color == WHITE) {
                // For white pawns, check if any black pawn is on higher ranks
                for (int check_rank = rank + 1; check_rank < 8; check_rank++) {
                    int check_square = make_square(check_file, check_rank);
                    if (file_pawns.test(check_square)) {
                        return false; // Found an opponent pawn ahead
                    }
                }
            } else {
                // For black pawns, check if any white pawn is on lower ranks
                for (int check_rank = rank - 1; check_rank >= 0; check_rank--) {
                    int check_square = make_square(check_file, check_rank);
                    if (file_pawns.test(check_square)) {
                        return false; // Found an opponent pawn ahead
                    }
                }
            }
        }
    }
    
    return true; // No opponent pawns ahead
}

} // namespace PawnStructure

} // namespace Cerberus 