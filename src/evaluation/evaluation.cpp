#include "evaluation/evaluation.h"
#include "evaluation/nnue_adapter.h"
#include "core/move_generator.h"

namespace Cerberus {

// Main evaluation function
Score Evaluation::evaluate(const Board& board) {
    Score nnue_score = NNUE_Adapter::evaluate(board);
    Score classical_bonus = 0;

    // Add classical evaluation terms as a bonus/penalty to the NNUE score
    classical_bonus += evaluate_mobility(board);
    classical_bonus += evaluate_king_safety(board);
    classical_bonus += evaluate_pawn_structure(board);
    classical_bonus += get_tempo_bonus(board);

    return nnue_score + classical_bonus;
}

// Material evaluation
Score Evaluation::evaluate_material(const Board& board) {
    return evaluate_material_balance(board);
}

Score Evaluation::evaluate_material_balance(const Board& board) {
    Score score = 0;
    
    // Count material for each side
    for (int square = 0; square < 64; square++) {
        Piece piece = board.get_piece(square);
        if (piece != EMPTY) {
            Color color = get_color(piece);
            PieceType type = get_piece_type(piece);
            
            Score piece_value = material_values[type - 1];
            if (color == BLACK) {
                piece_value = -piece_value;
            }
            score += piece_value;
        }
    }
    
    return score;
}

// Positional evaluation
Score Evaluation::evaluate_positional(const Board& board) {
    Score score = 0;
    score += evaluate_piece_position(board, WHITE);
    score -= evaluate_piece_position(board, BLACK);
    return score;
}

Score Evaluation::evaluate_piece_position(const Board& board, Color color) {
    Score score = 0;
    
    // Evaluate each piece type
    for (PieceType type = PAWN; type <= KING; type = static_cast<PieceType>(type + 1)) {
        Bitboard pieces = board.get_piece_bitboard(color, type);
        while (pieces) {
            int square = pieces.pop_lsb();
            score += positional_weights[type - 1]; // Simplified positional scoring
        }
    }
    
    return score;
}

// Pawn structure evaluation
Score Evaluation::evaluate_pawn_structure(const Board& board) {
    Score score = 0;
    score += evaluate_pawn_structure_color(board, WHITE);
    score -= evaluate_pawn_structure_color(board, BLACK);
    return score;
}

Score Evaluation::evaluate_pawn_structure_color(const Board& board, Color color) {
    Score score = 0;
    Bitboard pawns = board.get_piece_bitboard(color, PAWN);
    
    // Count various pawn features
    score += count_isolated_pawns(board, color) * -10;
    score += count_doubled_pawns(board, color) * -5;
    score += count_passed_pawns(board, color) * 20;
    
    return score;
}

// Mobility evaluation
Score Evaluation::evaluate_mobility(const Board& board) {
    Score score = 0;
    score += evaluate_mobility_color(board, WHITE);
    score -= evaluate_mobility_color(board, BLACK);
    return score;
}

Score Evaluation::evaluate_mobility_color(const Board& board, Color color) {
    Score mobility_score = 0;
    MoveGenerator move_gen(board);
    auto moves = move_gen.generate_legal_moves();

    for (const auto& move : moves) {
        if (get_color(board.get_piece(move.get_from())) == color) {
            mobility_score++;
        }
    }
    return mobility_score;
}

// King safety evaluation
Score Evaluation::evaluate_king_safety(const Board& board) {
    Score score = 0;
    score += evaluate_king_safety_color(board, WHITE);
    score -= evaluate_king_safety_color(board, BLACK);
    return score;
}

Score Evaluation::evaluate_king_safety_color(const Board& board, Color color) {
    Score score = 0;
    int king_square = board.get_king_square(color);
    int king_file = king_square % 8;
    int king_rank = king_square / 8;

    // Penalty for open files near the king
    for (int f = king_file - 1; f <= king_file + 1; ++f) {
        if (f < 0 || f > 7) continue;

        bool file_is_open = true;
        for (int r = 0; r < 8; ++r) {
            if (get_piece_type(board.get_piece(r * 8 + f)) == PAWN) {
                file_is_open = false;
                break;
            }
        }
        if (file_is_open) {
            score -= 20;
        }

        bool file_is_semi_open = true;
        Bitboard pawns = board.get_piece_bitboard(color, PAWN);
        if ((pawns & Bitboard::file_mask(f)) == 0) {
            score -= 10;
        }
    }

    // Bonus for pawn shield
    int shield_rank = (color == WHITE) ? king_rank + 1 : king_rank - 1;
    if (shield_rank > 0 && shield_rank < 7) {
        for (int f = king_file - 1; f <= king_file + 1; ++f) {
            if (f < 0 || f > 7) continue;
            Piece piece = board.get_piece(shield_rank * 8 + f);
            if (get_piece_type(piece) == PAWN && get_color(piece) == color) {
                score += 10;
            }
        }
    }

    return score;
}

// Game phase detection
int Evaluation::get_game_phase(const Board& board) {
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

Score Evaluation::interpolate_score(Score opening_score, Score endgame_score, int game_phase) {
    // Simplified interpolation
    return (opening_score + endgame_score) / 2;
}

// Tempo bonus
Score Evaluation::get_tempo_bonus(const Board& board) {
    return board.get_side_to_move() == WHITE ? 10 : -10;
}

// Evaluation tuning
void Evaluation::set_material_values(const int values[6]) {
    for (int i = 0; i < 6; i++) {
        material_values[i] = values[i];
    }
}

void Evaluation::set_positional_weights(const int weights[6]) {
    for (int i = 0; i < 6; i++) {
        positional_weights[i] = weights[i];
    }
}

// Static member initialization
int Evaluation::material_values[6] = {100, 320, 330, 500, 900, 20000}; // P, N, B, R, Q, K
int Evaluation::positional_weights[6] = {0, 0, 0, 0, 0, 0}; // Simplified

// Helper functions
bool Evaluation::is_isolated_pawn(const Board& board, int square, Color color) {
    // Simplified implementation
    return false;
}

bool Evaluation::is_doubled_pawn(const Board& board, int square, Color color) {
    // Simplified implementation
    return false;
}

bool Evaluation::is_backward_pawn(const Board& board, int square, Color color) {
    // Simplified implementation
    return false;
}

bool Evaluation::is_passed_pawn(const Board& board, int square, Color color) {
    // Simplified implementation
    return false;
}

int Evaluation::count_pawn_islands(const Board& board, Color color) {
    // Simplified implementation
    return 0;
}

int Evaluation::count_passed_pawns(const Board& board, Color color) {
    // Simplified implementation
    return 0;
}

int Evaluation::count_doubled_pawns(const Board& board, Color color) {
    // Simplified implementation
    return 0;
}

int Evaluation::count_isolated_pawns(const Board& board, Color color) {
    // Simplified implementation
    return 0;
}

Score Evaluation::evaluate_king_distance(const Board& board, Color color) {
    // Simplified implementation
    return 0;
}

Score Evaluation::evaluate_center_control(const Board& board, Color color) {
    // Simplified implementation
    return 0;
}

Score Evaluation::evaluate_development(const Board& board, Color color) {
    // Simplified implementation
    return 0;
}

} // namespace Cerberus
