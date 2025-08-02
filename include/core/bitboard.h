#pragma once
#include "types.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <cstdint>
#include <cstdlib>
#include <string>

namespace Cerberus {

namespace Bitboards {

void        init();
void        init_magics();
std::string pretty(Bitboard b);

// Helper functions for magic bitboard generation
Bitboard get_rook_rays(Square sq);
Bitboard get_bishop_rays(Square sq);
Bitboard get_border(Square sq);
Bitboard get_blockers(Square sq, int blocker_index, Bitboard mask);

}  // namespace Cerberus::Bitboards

// File bitboards
constexpr Bitboard FileABB = 0x0101010101010101ULL;
constexpr Bitboard FileBBB = FileABB << 1;
constexpr Bitboard FileCBB = FileABB << 2;
constexpr Bitboard FileDBB = FileABB << 3;
constexpr Bitboard FileEBB = FileABB << 4;
constexpr Bitboard FileFBB = FileABB << 5;
constexpr Bitboard FileGBB = FileABB << 6;
constexpr Bitboard FileHBB = FileABB << 7;

// Rank bitboards
constexpr Bitboard Rank1BB = 0xFF;
constexpr Bitboard Rank2BB = Rank1BB << (8 * 1);
constexpr Bitboard Rank3BB = Rank1BB << (8 * 2);
constexpr Bitboard Rank4BB = Rank1BB << (8 * 3);
constexpr Bitboard Rank5BB = Rank1BB << (8 * 4);
constexpr Bitboard Rank6BB = Rank1BB << (8 * 5);
constexpr Bitboard Rank7BB = Rank1BB << (8 * 6);
constexpr Bitboard Rank8BB = Rank1BB << (8 * 7);

// External arrays
extern uint8_t PopCnt16[1 << 16];
extern uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];

extern Bitboard BetweenBB[SQUARE_NB][SQUARE_NB];
extern Bitboard LineBB[SQUARE_NB][SQUARE_NB];
extern Bitboard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];

// Magic holds all magic bitboards relevant data for a single square
struct Magic {
    Bitboard  mask;
    Bitboard* attacks;
    Bitboard magic;
    unsigned shift;

    // Compute the attack's index using the 'magic bitboards' approach
    unsigned index(Bitboard occupied) const {
        return unsigned(((occupied & mask) * magic) >> shift);
    }

    Bitboard attacks_bb(Bitboard occupied) const { return attacks[index(occupied)]; }
};

extern Magic Magics[SQUARE_NB][2];

constexpr Bitboard square_bb(Square s) {
    assert(is_ok(s));
    return (1ULL << s);
}

// Overloads of bitwise operators between a Bitboard and a Square
constexpr Bitboard  operator&(Bitboard b, Square s) { return b & square_bb(s); }
constexpr Bitboard  operator|(Bitboard b, Square s) { return b | square_bb(s); }
constexpr Bitboard  operator^(Bitboard b, Square s) { return b ^ square_bb(s); }
constexpr Bitboard& operator|=(Bitboard& b, Square s) { return b |= square_bb(s); }
constexpr Bitboard& operator^=(Bitboard& b, Square s) { return b ^= square_bb(s); }

constexpr Bitboard operator&(Square s, Bitboard b) { return b & s; }
constexpr Bitboard operator|(Square s, Bitboard b) { return b | s; }
constexpr Bitboard operator^(Square s, Bitboard b) { return b ^ s; }

constexpr Bitboard operator|(Square s1, Square s2) { return square_bb(s1) | s2; }

constexpr bool more_than_one(Bitboard b) { return b & (b - 1); }

// rank_bb() and file_bb() return a bitboard representing all the squares on
// the given file or rank.
constexpr Bitboard rank_bb(Rank r) { return Rank1BB << (8 * r); }
constexpr Bitboard rank_bb(Square s) { return rank_bb(rank_of(s)); }
constexpr Bitboard file_bb(File f) { return FileABB << f; }
constexpr Bitboard file_bb(Square s) { return file_bb(file_of(s)); }

// Moves a bitboard one or two steps as specified by the direction D
template<Direction D>
constexpr Bitboard shift(Bitboard b) {
    return D == NORTH         ? b << 8
         : D == SOUTH         ? b >> 8
         : D == NORTH + NORTH ? b << 16
         : D == SOUTH + SOUTH ? b >> 16
         : D == EAST          ? (b & ~FileHBB) << 1
         : D == WEST          ? (b & ~FileABB) >> 1
         : D == NORTH_EAST    ? (b & ~FileHBB) << 9
         : D == NORTH_WEST    ? (b & ~FileABB) << 7
         : D == SOUTH_EAST    ? (b & ~FileHBB) >> 7
         : D == SOUTH_WEST    ? (b & ~FileABB) >> 9
                              : 0;
}

// Returns the squares attacked by pawns of the given color
// from the squares in the given bitboard.
constexpr Bitboard pawn_attacks_bb(Bitboard b) {
    return shift<NORTH_EAST>(b) | shift<NORTH_WEST>(b);
}

constexpr Bitboard pawn_attacks_bb(Color c, Bitboard b) {
    return c == WHITE ? pawn_attacks_bb(b) : shift<SOUTH_EAST>(b) | shift<SOUTH_WEST>(b);
}

// Returns a bitboard representing all squares between s1 and s2, exclusive.
// If s1 and s2 are not on the same line, file or diagonal, the empty bitboard is returned.
inline Bitboard between_bb(Square s1, Square s2) {
    return BetweenBB[s1][s2];
}

// Returns a bitboard representing all squares along the line s1-s2, exclusive.
// If s1 and s2 are not on the same line, file or diagonal, the empty bitboard is returned.
inline Bitboard line_bb(Square s1, Square s2) {
    return LineBB[s1][s2];
}

// Returns true if the three squares s1, s2, s3 are aligned either on a
// straight line (file, rank, diagonal) or on a knight's path.
inline bool aligned(Square s1, Square s2, Square s3) { 
    return line_bb(s1, s2) & s3; 
}

// Distance functions
inline int distance(Square x, Square y) { 
    return SquareDistance[x][y]; 
}

inline int edge_distance(File f) { 
    return std::min(f, File(FILE_H - f)); 
}

// Returns the bitboard of attacked squares by the given piece type
// from the given square, assuming the board is empty except for the piece itself.
inline Bitboard attacks_bb(Square s, Color c = COLOR_NB) {
    return PseudoAttacks[c == COLOR_NB ? PAWN : PAWN][s];
}

// Returns the bitboard of attacked squares by the given piece type
// from the given square, assuming the board is occupied according to
// the passed bitboard.
inline Bitboard attacks_bb(Square s, Bitboard occupied) {
    return PseudoAttacks[PAWN][s];
}

// Returns the bitboard of attacked squares by the given piece type
// from the given square, assuming the board is occupied according to
// the passed bitboard.
inline Bitboard attacks_bb(PieceType pt, Square s, Bitboard occupied) {
    switch (pt) {
        case ROOK:   return Magics[s][0].attacks_bb(occupied);
        case BISHOP: return Magics[s][1].attacks_bb(occupied);
        default:     return PseudoAttacks[pt][s];
    }
}

// Returns the number of set bits in the bitboard
inline int popcount(Bitboard b) {
    return PopCnt16[b & 0xFFFF] + PopCnt16[(b >> 16) & 0xFFFF] + 
           PopCnt16[(b >> 32) & 0xFFFF] + PopCnt16[(b >> 48) & 0xFFFF];
}

// Returns the least significant bit in the bitboard
inline Square lsb(Bitboard b) {
    assert(b);
    return Square(__builtin_ctzll(b));
}

// Returns the most significant bit in the bitboard
inline Square msb(Bitboard b) {
    assert(b);
    return Square(63 - __builtin_clzll(b));
}

// Returns the least significant square in the bitboard
inline Bitboard least_significant_square_bb(Bitboard b) {
    return b & -b;
}

// Pops the least significant bit from the bitboard and returns the square
inline Square pop_lsb(Bitboard& b) {
    Square s = lsb(b);
    b &= b - 1;
    return s;
}

} // namespace Cerberus
