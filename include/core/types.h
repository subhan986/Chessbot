#pragma once
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace Cerberus {

// Basic types
using Key = uint64_t;
using Value = int;
using Score = int;
using Time = int;

// Constants
constexpr Value VALUE_INFINITE = 32001;
constexpr Value VALUE_MATE = 32000;
constexpr Value VALUE_MATE_IN_MAX_PLY = VALUE_MATE - 1000;
constexpr Value VALUE_MATED_IN_MAX_PLY = -VALUE_MATE_IN_MAX_PLY;
constexpr Value VALUE_NONE = 32002;
constexpr Value VALUE_TB = VALUE_MATE_IN_MAX_PLY;
constexpr Value VALUE_TB_WIN_IN_MAX_PLY = VALUE_TB - 1;
constexpr Value VALUE_TB_LOSS_IN_MAX_PLY = -VALUE_TB_WIN_IN_MAX_PLY;

// Colors
enum Color : int8_t {
    WHITE,
    BLACK,
    COLOR_NB = 2
};

// Castling rights
enum CastlingRights : int8_t {
    NO_CASTLING,
    WHITE_OO,
    WHITE_OOO = WHITE_OO << 1,
    BLACK_OO  = WHITE_OO << 2,
    BLACK_OOO = WHITE_OO << 3,

    KING_SIDE      = WHITE_OO | BLACK_OO,
    QUEEN_SIDE     = WHITE_OOO | BLACK_OOO,
    WHITE_CASTLING = WHITE_OO | WHITE_OOO,
    BLACK_CASTLING = BLACK_OO | BLACK_OOO,
    ANY_CASTLING   = WHITE_CASTLING | BLACK_CASTLING,

    CASTLING_RIGHT_NB = 16
};

// Bound for transposition table
enum Bound : int8_t {
    BOUND_NONE,
    BOUND_UPPER,
    BOUND_LOWER,
    BOUND_EXACT = BOUND_UPPER | BOUND_LOWER
};

// Value utilities
constexpr bool is_valid(Value value) { return value != VALUE_NONE; }

constexpr bool is_win(Value value) {
    return value >= VALUE_MATE_IN_MAX_PLY;
}

constexpr bool is_loss(Value value) {
    return value <= VALUE_MATED_IN_MAX_PLY;
}

constexpr bool is_decisive(Value value) { 
    return is_win(value) || is_loss(value); 
}

constexpr Value mate_in(int ply) { 
    return VALUE_MATE - ply; 
}

constexpr Value mated_in(int ply) { 
    return -VALUE_MATE + ply; 
}

// Piece types
enum PieceType : std::int8_t {
    NO_PIECE_TYPE, PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    ALL_PIECES = 0,
    PIECE_TYPE_NB = 8
};

// Pieces
enum Piece : std::int8_t {
    NO_PIECE,
    W_PAWN = PAWN,     W_KNIGHT, W_BISHOP, W_ROOK, W_QUEEN, W_KING,
    B_PAWN = PAWN + 8, B_KNIGHT, B_BISHOP, B_ROOK, B_QUEEN, B_KING,
    PIECE_NB = 16
};

// Squares
enum Square : int8_t {
    SQ_A1, SQ_B1, SQ_C1, SQ_D1, SQ_E1, SQ_F1, SQ_G1, SQ_H1,
    SQ_A2, SQ_B2, SQ_C2, SQ_D2, SQ_E2, SQ_F2, SQ_G2, SQ_H2,
    SQ_A3, SQ_B3, SQ_C3, SQ_D3, SQ_E3, SQ_F3, SQ_G3, SQ_H3,
    SQ_A4, SQ_B4, SQ_C4, SQ_D4, SQ_E4, SQ_F4, SQ_G4, SQ_H4,
    SQ_A5, SQ_B5, SQ_C5, SQ_D5, SQ_E5, SQ_F5, SQ_G5, SQ_H5,
    SQ_A6, SQ_B6, SQ_C6, SQ_D6, SQ_E6, SQ_F6, SQ_G6, SQ_H6,
    SQ_A7, SQ_B7, SQ_C7, SQ_D7, SQ_E7, SQ_F7, SQ_G7, SQ_H7,
    SQ_A8, SQ_B8, SQ_C8, SQ_D8, SQ_E8, SQ_F8, SQ_G8, SQ_H8,
    SQ_NONE,

    SQUARE_ZERO = 0,
    SQUARE_NB   = 64
};

// Directions
enum Direction : int8_t {
    NORTH = 8,
    EAST  = 1,
    SOUTH = -NORTH,
    WEST  = -EAST,

    NORTH_EAST = NORTH + EAST,
    SOUTH_EAST = SOUTH + EAST,
    SOUTH_WEST = SOUTH + WEST,
    NORTH_WEST = NORTH + WEST
};

// Files
enum File : int8_t {
    FILE_A,
    FILE_B,
    FILE_C,
    FILE_D,
    FILE_E,
    FILE_F,
    FILE_G,
    FILE_H,
    FILE_NB
};

// Ranks
enum Rank : int8_t {
    RANK_1,
    RANK_2,
    RANK_3,
    RANK_4,
    RANK_5,
    RANK_6,
    RANK_7,
    RANK_8,
    RANK_NB
};

// Bitboard type
using Bitboard = uint64_t;

// Move types
enum MoveType {
    NORMAL,
    PROMOTION  = 1 << 14,
    EN_PASSANT = 2 << 14,
    CASTLING   = 3 << 14
};

// Move class
class Move {
private:
    std::uint16_t data;
    Score score;

public:
    constexpr explicit Move(std::uint16_t d) : data(d), score(0) {}
    constexpr Move(Square from, Square to) : data((from << 6) + to), score(0) {}

    template<MoveType T>
    static constexpr Move make(Square from, Square to, PieceType pt = KNIGHT) {
        return Move(T + ((pt - KNIGHT) << 12) + (from << 6) + to);
    }

    constexpr Square from_sq() const { return Square((data >> 6) & 0x3F); }
    constexpr Square to_sq() const { return Square(data & 0x3F); }
    constexpr int from_to() const { return data & 0xFFF; }
    constexpr MoveType type_of() const { return MoveType(data & (3 << 14)); }
    constexpr PieceType promotion_type() const { return PieceType(((data >> 12) & 3) + KNIGHT); }

    constexpr bool is_ok() const { return none().data != data && null().data != data; }

    static constexpr Move null() { return Move(65); }
    static constexpr Move none() { return Move(0); }

    constexpr bool operator==(const Move& m) const { return data == m.data; }
    constexpr bool operator!=(const Move& m) const { return data != m.data; }
    constexpr explicit operator bool() const { return data != 0; }
    constexpr std::uint16_t raw() const { return data; }

    void set_score(Score s) { score = s; }
    Score get_score() const { return score; }
};

// Utility functions
constexpr Square make_square(File f, Rank r) { return Square((r << 3) + f); }
constexpr Piece make_piece(Color c, PieceType pt) { return Piece((c << 3) + pt); }
constexpr PieceType type_of(Piece pc) { return PieceType(pc & 7); }
constexpr Color color_of(Piece pc) { return Color(pc >> 3); }
constexpr bool is_ok(Square s) { return s >= SQ_A1 && s <= SQ_H8; }
constexpr File file_of(Square s) { return File(s & 7); }
constexpr Rank rank_of(Square s) { return Rank(s >> 3); }
constexpr Square relative_square(Color c, Square s) { return Square(s ^ (c * 56)); }
constexpr Rank relative_rank(Color c, Rank r) { return Rank(r ^ (c * 7)); }
constexpr Rank relative_rank(Color c, Square s) { return relative_rank(c, rank_of(s)); }
constexpr Direction pawn_push(Color c) { return c == WHITE ? NORTH : SOUTH; }

// Increment operators
template<typename T>
constexpr T& operator++(T& d) { return d = T(int(d) + 1); }

template<typename T>
constexpr T& operator--(T& d) { return d = T(int(d) - 1); }

template<typename T>
constexpr T operator++(T& d, int) { T t = d; ++d; return t; }

template<typename T>
constexpr T operator--(T& d, int) { T t = d; --d; return t; }



// Direction operators
constexpr Direction operator+(Direction d1, Direction d2) { return Direction(int(d1) + int(d2)); }
constexpr Direction operator-(Direction d1, Direction d2) { return Direction(int(d1) - int(d2)); }
constexpr Direction operator-(Direction d) { return Direction(-int(d)); }

// Square arithmetic operators
constexpr Square operator+(Square s, Direction d) { return Square(int(s) + int(d)); }
constexpr Square operator-(Square s, Direction d) { return Square(int(s) - int(d)); }
constexpr Square operator+=(Square& s, Direction d) { return s = Square(int(s) + int(d)); }
constexpr Square operator-=(Square& s, Direction d) { return s = Square(int(s) - int(d)); }

// Square utilities
constexpr Square flip_rank(Square s) { return Square(s ^ SQ_A8); }
constexpr Square flip_file(Square s) { return Square(s ^ SQ_H1); }

// Key generation
constexpr Key make_key(uint64_t seed) {
    return seed * 0x00000100000001B3ULL;
}

} // namespace Cerberus
