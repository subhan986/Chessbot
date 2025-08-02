#include "core/bitboard.h"
#include "core/types.h"
#include <iostream>

namespace Cerberus {

namespace Bitboards {

// A global variable to store the magic numbers.
// This is not ideal, but it's a simple way to get things working.
Magic Magics[SQUARE_NB][2];

// A simple random number generator.
uint64_t rng() {
    uint64_t r = 0;
    for (int i = 0; i < 4; ++i) {
        r = (r << 15) | (rand() & 0x7FFF);
    }
    return r;
}

// A function to generate the attack bitboard for a given square and occupied bitboard.
Bitboard rook_attacks(Square sq, Bitboard occupied) {
    Bitboard attacks = 0;
    int r, f;
    int rk = rank_of(sq);
    int fl = file_of(sq);

    for (r = rk + 1; r <= 7; ++r) {
        attacks |= (1ULL << (r * 8 + fl));
        if (occupied & (1ULL << (r * 8 + fl))) break;
    }
    for (r = rk - 1; r >= 0; --r) {
        attacks |= (1ULL << (r * 8 + fl));
        if (occupied & (1ULL << (r * 8 + fl))) break;
    }
    for (f = fl + 1; f <= 7; ++f) {
        attacks |= (1ULL << (rk * 8 + f));
        if (occupied & (1ULL << (rk * 8 + f))) break;
    }
    for (f = fl - 1; f >= 0; --f) {
        attacks |= (1ULL << (rk * 8 + f));
        if (occupied & (1ULL << (rk * 8 + f))) break;
    }
    return attacks;
}

Bitboard bishop_attacks(Square sq, Bitboard occupied) {
    Bitboard attacks = 0;
    int r, f;
    int rk = rank_of(sq);
    int fl = file_of(sq);

    for (r = rk + 1, f = fl + 1; r <= 7 && f <= 7; ++r, ++f) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) break;
    }
    for (r = rk + 1, f = fl - 1; r <= 7 && f >= 0; ++r, --f) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) break;
    }
    for (r = rk - 1, f = fl + 1; r >= 0 && f <= 7; --r, ++f) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) break;
    }
    for (r = rk - 1, f = fl - 1; r >= 0 && f >= 0; --r, --f) {
        attacks |= (1ULL << (r * 8 + f));
        if (occupied & (1ULL << (r * 8 + f))) break;
    }
    return attacks;
}

// A function to find a magic number for a given square and piece type.
void find_magic(Square sq, bool is_bishop) {
    Bitboard mask, attacks[4096], used[4096];
    int i, j, n;

    mask = is_bishop ? (get_bishop_rays(sq) & ~get_border(sq)) : (get_rook_rays(sq) & ~get_border(sq));
    n = popcount(mask);

    for (i = 0; i < (1 << n); ++i) {
        attacks[i] = is_bishop ? bishop_attacks(sq, get_blockers(sq, i, mask)) : rook_attacks(sq, get_blockers(sq, i, mask));
    }

    for (j = 0; j < 100000000; ++j) {
        Magics[sq][is_bishop].magic = rng() & rng() & rng();
        if (popcount((mask * Magics[sq][is_bishop].magic) & 0xFF00000000000000ULL) < 6) continue;

        std::fill(used, used + 4096, 0);
        for (i = 0; i < (1 << n); ++i) {
            int idx = (get_blockers(sq, i, mask) * Magics[sq][is_bishop].magic) >> (64 - n);
            if (used[idx] == 0) {
                used[idx] = attacks[i];
            } else if (used[idx] != attacks[i]) {
                break;
            }
        }
        if (i == (1 << n)) {
            Magics[sq][is_bishop].mask = mask;
            Magics[sq][is_bishop].shift = 64 - n;
            Magics[sq][is_bishop].attacks = new Bitboard[1 << n];
            for (i = 0; i < (1 << n); ++i) {
                int idx = (get_blockers(sq, i, mask) * Magics[sq][is_bishop].magic) >> (64 - n);
                Magics[sq][is_bishop].attacks[idx] = attacks[i];
            }
            return;
        }
    }
    std::cout << "Failed to find magic number for " << (is_bishop ? "bishop" : "rook") << " on " << sq << std::endl;
}

// A function to initialize the magic bitboards.
void init_magics() {
    for (int sq = 0; sq < 64; ++sq) {
        find_magic(static_cast<Square>(sq), false);
        find_magic(static_cast<Square>(sq), true);
    }
}

} // namespace Bitboards
} // namespace Cerberus
