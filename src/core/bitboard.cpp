#include "core/bitboard.h"
#include <cstring>
#include <sstream>

namespace Cerberus {

namespace Bitboards {

// Initialize bitboard tables
uint8_t PopCnt16[1 << 16];
uint8_t SquareDistance[SQUARE_NB][SQUARE_NB];
Bitboard BetweenBB[SQUARE_NB][SQUARE_NB];
Bitboard LineBB[SQUARE_NB][SQUARE_NB];
Bitboard PseudoAttacks[PIECE_TYPE_NB][SQUARE_NB];
Magic Magics[SQUARE_NB][2];

// Initialize popcount table
void init_popcount() {
    for (unsigned i = 0; i < (1 << 16); ++i) {
        PopCnt16[i] = __builtin_popcount(i);
    }
}

// Distance template function
template<typename T>
constexpr int distance(T x, T y) {
    return std::abs(int(x) - int(y));
}

// Initialize square distance table
void init_square_distance() {
    for (Square s1 = SQ_A1; s1 <= SQ_H8; ++s1) {
        for (Square s2 = SQ_A1; s2 <= SQ_H8; ++s2) {
            SquareDistance[s1][s2] = std::max(distance<File>(file_of(s1), file_of(s2)),
                                             distance<Rank>(rank_of(s1), rank_of(s2)));
        }
    }
}

// Initialize between and line bitboards
void init_between_line() {
    for (Square s1 = SQ_A1; s1 <= SQ_H8; ++s1) {
        for (Square s2 = SQ_A1; s2 <= SQ_H8; ++s2) {
            BetweenBB[s1][s2] = 0;
            LineBB[s1][s2] = 0;
            
            if (s1 == s2) continue;
            
            // Same file
            if (file_of(s1) == file_of(s2)) {
                Bitboard b = file_bb(s1) & ~square_bb(s1) & ~square_bb(s2);
                LineBB[s1][s2] = file_bb(s1);
                BetweenBB[s1][s2] = b;
            }
            // Same rank
            else if (rank_of(s1) == rank_of(s2)) {
                Bitboard b = rank_bb(s1) & ~square_bb(s1) & ~square_bb(s2);
                LineBB[s1][s2] = rank_bb(s1);
                BetweenBB[s1][s2] = b;
            }
            // Same diagonal
            else if (distance<File>(file_of(s1), file_of(s2)) == distance<Rank>(rank_of(s1), rank_of(s2))) {
                Bitboard b = 0;
                Direction d = (s2 > s1) ? NORTH_EAST : SOUTH_WEST;
                if (file_of(s2) < file_of(s1)) d = (s2 > s1) ? NORTH_WEST : SOUTH_EAST;
                
                for (Square s = s1 + d; s != s2; s += d) {
                    if (is_ok(s)) b |= square_bb(s);
                }
                BetweenBB[s1][s2] = b;
                
                // Line includes both squares
                b = square_bb(s1) | square_bb(s2);
                for (Square s = s1 + d; is_ok(s) && s != s2; s += d) b |= square_bb(s);
                for (Square s = s2 - d; is_ok(s) && s != s1; s -= d) b |= square_bb(s);
                LineBB[s1][s2] = b;
            }
        }
    }
}

// Initialize pseudo-attack tables
void init_pseudo_attacks() {
    // Initialize all to 0
    std::memset(PseudoAttacks, 0, sizeof(PseudoAttacks));
    
    // Pawn attacks
    for (Square s = SQ_A1; s <= SQ_H8; ++s) {
        if (rank_of(s) < RANK_8) {
            PseudoAttacks[PAWN][s] = pawn_attacks_bb(WHITE, square_bb(s));
        }
    }
    
    // Knight attacks
    for (Square s = SQ_A1; s <= SQ_H8; ++s) {
        Bitboard b = square_bb(s);
        PseudoAttacks[KNIGHT][s] = shift<NORTH + NORTH + EAST>(b) | shift<NORTH + NORTH + WEST>(b) |
                                   shift<SOUTH + SOUTH + EAST>(b) | shift<SOUTH + SOUTH + WEST>(b) |
                                   shift<EAST + EAST + NORTH>(b) | shift<EAST + EAST + SOUTH>(b) |
                                   shift<WEST + WEST + NORTH>(b) | shift<WEST + WEST + SOUTH>(b);
    }
    
    // King attacks
    for (Square s = SQ_A1; s <= SQ_H8; ++s) {
        Bitboard b = square_bb(s);
        PseudoAttacks[KING][s] = shift<NORTH>(b) | shift<SOUTH>(b) | shift<EAST>(b) | shift<WEST>(b) |
                                 shift<NORTH_EAST>(b) | shift<NORTH_WEST>(b) | shift<SOUTH_EAST>(b) | shift<SOUTH_WEST>(b);
    }
}

// Initialize magic bitboards
void init_magic_bitboards() {
    init_magics();
}

// Pretty print bitboard
std::string pretty(Bitboard b) {
    std::stringstream ss;
    ss << "\n";
    for (Rank r = RANK_8; r >= RANK_1; --r) {
        for (File f = FILE_A; f <= FILE_H; ++f) {
            Square s = make_square(f, r);
            ss << (b & square_bb(s) ? " 1" : " .");
        }
        ss << "\n";
    }
    ss << "\n";
    return ss.str();
}

// Main initialization function
void init() {
    init_popcount();
    init_square_distance();
    init_between_line();
    init_pseudo_attacks();
    init_magic_bitboards();
}

// Helper functions for magic bitboard generation
Bitboard get_rook_rays(Square sq) {
    return rank_bb(sq) | file_bb(sq);
}

Bitboard get_bishop_rays(Square sq) {
    Bitboard b = 0;
    int r = rank_of(sq);
    int f = file_of(sq);
    for (int i = -7; i <= 7; ++i) {
        if (r + i >= 0 && r + i <= 7 && f + i >= 0 && f + i <= 7)
            b |= square_bb(make_square(File(f + i), Rank(r + i)));
        if (r + i >= 0 && r + i <= 7 && f - i >= 0 && f - i <= 7)
            b |= square_bb(make_square(File(f - i), Rank(r + i)));
    }
    return b;
}

Bitboard get_border(Square sq) {
    return (file_bb(FILE_A) | file_bb(FILE_H) | rank_bb(RANK_1) | rank_bb(RANK_8)) & ~square_bb(sq);
}

Bitboard get_blockers(Square sq, int blocker_index, Bitboard mask) {
    Bitboard blockers = 0;
    Bitboard m = mask;
    while (m) {
        Square s = pop_lsb(m);
        if (blocker_index & 1) {
            blockers |= square_bb(s);
        }
        blocker_index >>= 1;
    }
    return blockers;
}

} // namespace Bitboards

} // namespace Cerberus
