#pragma once

#include "types.h"
#include "board.h"
#include "move.h"
#include <vector>

namespace Cerberus {

class MoveGenerator {
public:
    // Generate all legal moves
    static void generate_moves(const Position& pos, std::vector<Move>& moves);
    
    // Generate only captures
    static void generate_captures(const Position& pos, std::vector<Move>& moves);
    
    // Generate only non-captures
    static void generate_quiets(const Position& pos, std::vector<Move>& moves);
    
    // Generate moves for specific piece types
    static void generate_pawn_moves(const Position& pos, std::vector<Move>& moves);
    static void generate_knight_moves(const Position& pos, std::vector<Move>& moves);
    static void generate_bishop_moves(const Position& pos, std::vector<Move>& moves);
    static void generate_rook_moves(const Position& pos, std::vector<Move>& moves);
    static void generate_queen_moves(const Position& pos, std::vector<Move>& moves);
    static void generate_king_moves(const Position& pos, std::vector<Move>& moves);
    
    // Generate castling moves
    static void generate_castling_moves(const Position& pos, std::vector<Move>& moves);
    
    // Generate en passant moves
    static void generate_en_passant_moves(const Position& pos, std::vector<Move>& moves);
    
    // Check if a move is legal
    static bool is_legal_move(const Position& pos, Move move);
    
    // Check if position is in check
    static bool is_in_check(const Position& pos);
    
    // Check if a square is attacked
    static bool is_square_attacked(const Position& pos, Square square, Color by_color);
    
private:
    // Helper functions for move generation
    static Bitboard get_pawn_attacks(Square square, Color color);
    static Bitboard get_knight_attacks(Square square);
    static Bitboard get_bishop_attacks(Square square, Bitboard occupied);
    static Bitboard get_rook_attacks(Square square, Bitboard occupied);
    static Bitboard get_queen_attacks(Square square, Bitboard occupied);
    static Bitboard get_king_attacks(Square square);
    
    // Magic bitboard tables (simplified)
    static Bitboard bishop_magic_table[64][512];
    static Bitboard rook_magic_table[64][4096];
    static uint64_t bishop_magic_numbers[64];
    static uint64_t rook_magic_numbers[64];
    
    // Initialize magic bitboards
    static void init_magic_bitboards();
};

} // namespace Cerberus
