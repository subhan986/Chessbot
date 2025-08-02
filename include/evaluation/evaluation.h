#pragma once

#include "../core/board.h"
#include "../core/types.h"

namespace Cerberus {

class Evaluation {
public:
    // Main evaluation function
    static Score evaluate(const Board& board);
    
    // Material evaluation
    static Score evaluate_material(const Board& board);
    static Score evaluate_material_balance(const Board& board);
    
    // Positional evaluation
    static Score evaluate_positional(const Board& board);
    static Score evaluate_piece_position(const Board& board, Color color);
    
    // Pawn structure evaluation
    static Score evaluate_pawn_structure(const Board& board);
    static Score evaluate_pawn_structure_color(const Board& board, Color color);
    
    // Mobility evaluation
    static Score evaluate_mobility(const Board& board);
    static Score evaluate_mobility_color(const Board& board, Color color);
    
    // King safety evaluation
    static Score evaluate_king_safety(const Board& board);
    static Score evaluate_king_safety_color(const Board& board, Color color);
    
    // Game phase detection
    static int get_game_phase(const Board& board);
    static Score interpolate_score(Score opening_score, Score endgame_score, int game_phase);
    
    // Tempo bonus
    static Score get_tempo_bonus(const Board& board);
    
    // Evaluation tuning
    static void set_material_values(const int values[6]);
    static void set_positional_weights(const int weights[6]);
    
private:
    // Evaluation weights
    static int material_values[6];
    static int positional_weights[6];
    
    // Helper functions
    static bool is_isolated_pawn(const Board& board, int square, Color color);
    static bool is_doubled_pawn(const Board& board, int square, Color color);
    static bool is_backward_pawn(const Board& board, int square, Color color);
    static bool is_passed_pawn(const Board& board, int square, Color color);
    
    static int count_pawn_islands(const Board& board, Color color);
    static int count_passed_pawns(const Board& board, Color color);
    static int count_doubled_pawns(const Board& board, Color color);
    static int count_isolated_pawns(const Board& board, Color color);
    
    static Score evaluate_king_distance(const Board& board, Color color);
    static Score evaluate_center_control(const Board& board, Color color);
    static Score evaluate_development(const Board& board, Color color);
};

} // namespace Cerberus 