#pragma once

#include "types.h"
#include "bitboard.h"
#include "move.h"
#include <string>
#include <vector>

namespace Cerberus {

struct BoardState {
    uint64_t zobrist_key;
    int castling_rights;
    int en_passant_square;
    int halfmove_clock;
    int fullmove_number;
    Move last_move;
    Piece captured_piece;
};

class Board {
private:
    // Board representation
    Piece squares[64];
    Bitboard piece_bitboards[2][7]; // [color][piece_type]
    Bitboard all_pieces;
    
    // Game state
    Color side_to_move;
    int castling_rights;
    int en_passant_square;
    int halfmove_clock;
    int fullmove_number;
    
    // Move history for undo
    std::vector<BoardState> history;
    
    // Zobrist key
    uint64_t zobrist_key;

public:
    // Constructors
    Board();
    
    // Board setup
    void reset();
    bool set_from_fen(const std::string& fen);
    std::string get_fen() const;
    
    // Board state
    Piece get_piece(int square) const;
    void set_piece(int square, Piece piece);
    Bitboard get_piece_bitboard(Color color, PieceType type) const;
    Bitboard get_all_pieces() const { return all_pieces; }
    
    // Game state
    Color get_side_to_move() const { return side_to_move; }
    int get_castling_rights() const { return castling_rights; }
    int get_en_passant_square() const { return en_passant_square; }
    int get_halfmove_clock() const { return halfmove_clock; }
    int get_fullmove_number() const { return fullmove_number; }
    uint64_t get_zobrist_key() const { return zobrist_key; }
    
    // Move making
    bool make_move(Move move);
    void unmake_move();
    bool make_null_move();
    void unmake_null_move();
    
    // Position analysis
    bool is_in_check() const;
    bool is_in_check(Color color) const;
    bool is_checkmate() const;
    bool is_stalemate() const;
    bool is_draw() const;
    bool is_legal_move(Move move) const;
    Move get_smallest_attacker(Square s) const;
    
    // Utility
    void print_board() const;
    void update_bitboards();
    void handle_special_moves(Move move);
    
private:
    void update_zobrist_key();
    void save_state();
    void restore_state(const BoardState& state);
};

} // namespace Cerberus
