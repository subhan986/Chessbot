#include "core/move_generator.h"
#include "core/position.h"
#include "core/bitboard.h"
#include "core/move.h"
#include <vector>

namespace Cerberus {

void MoveGenerator::generate_moves(const Position& pos, std::vector<Move>& moves) {
    moves.clear();
    
    generate_pawn_moves(pos, moves);
    generate_knight_moves(pos, moves);
    generate_bishop_moves(pos, moves);
    generate_rook_moves(pos, moves);
    generate_queen_moves(pos, moves);
    generate_king_moves(pos, moves);
    generate_castling_moves(pos, moves);
    generate_en_passant_moves(pos, moves);
}

void MoveGenerator::generate_captures(const Position& pos, std::vector<Move>& moves) {
    moves.clear();
    
    std::vector<Move> all_moves;
    generate_moves(pos, all_moves);
    
    for (const auto& move : all_moves) {
        if (pos.capture(move)) {
            moves.push_back(move);
        }
    }
}

void MoveGenerator::generate_quiets(const Position& pos, std::vector<Move>& moves) {
    moves.clear();

    std::vector<Move> all_moves;
    generate_moves(pos, all_moves);

    for (const auto& move : all_moves) {
        if (!pos.capture(move)) {
            moves.push_back(move);
        }
    }
}

void MoveGenerator::generate_pawn_moves(const Position& pos, std::vector<Move>& moves) {
    Color us = pos.side_to_move();
    Color them = Color(us ^ 1);
    Bitboard pawns = pos.pieces(us, PAWN);
    
    while (pawns) {
        Square from = pop_lsb(pawns);
        
        // Pushes
        Square to = from + pawn_push(us);
        if (pos.empty(to)) {
            if (rank_of(from) == (us == WHITE ? RANK_7 : RANK_2)) {
                moves.push_back(Move::make<PROMOTION>(from, to, QUEEN));
                moves.push_back(Move::make<PROMOTION>(from, to, ROOK));
                moves.push_back(Move::make<PROMOTION>(from, to, BISHOP));
                moves.push_back(Move::make<PROMOTION>(from, to, KNIGHT));
            } else {
                moves.push_back(Move(from, to));
            }
            
            // Double pushes
            if (rank_of(from) == (us == WHITE ? RANK_2 : RANK_7)) {
                Square double_push_to = from + pawn_push(us) + pawn_push(us);
                if (pos.empty(double_push_to)) {
                    moves.push_back(Move(from, double_push_to));
                }
            }
        }
        
        // Captures
        Bitboard attacks = pawn_attacks_bb(us, square_bb(from));
        Bitboard targets = attacks & pos.pieces(them);
        while (targets) {
            Square to_cap = pop_lsb(targets);
            if (rank_of(from) == (us == WHITE ? RANK_7 : RANK_2)) {
                moves.push_back(Move::make<PROMOTION>(from, to_cap, QUEEN));
                moves.push_back(Move::make<PROMOTION>(from, to_cap, ROOK));
                moves.push_back(Move::make<PROMOTION>(from, to_cap, BISHOP));
                moves.push_back(Move::make<PROMOTION>(from, to_cap, KNIGHT));
            } else {
                moves.push_back(Move(from, to_cap));
            }
        }
    }
}

void MoveGenerator::generate_knight_moves(const Position& pos, std::vector<Move>& moves) {
    Color us = pos.side_to_move();
    Bitboard knights = pos.pieces(us, KNIGHT);
    
    while (knights) {
        Square from = pop_lsb(knights);
        Bitboard attacks = attacks_bb(KNIGHT, from, 0);
        Bitboard targets = attacks & ~pos.pieces(us);
        
        while (targets) {
            Square to = pop_lsb(targets);
            moves.push_back(Move(from, to));
        }
    }
}

void MoveGenerator::generate_bishop_moves(const Position& pos, std::vector<Move>& moves) {
    Color us = pos.side_to_move();
    Bitboard bishops = pos.pieces(us, BISHOP);
    
    while (bishops) {
        Square from = pop_lsb(bishops);
        Bitboard attacks = attacks_bb(BISHOP, from, pos.pieces());
        Bitboard targets = attacks & ~pos.pieces(us);
        
        while (targets) {
            Square to = pop_lsb(targets);
            moves.push_back(Move(from, to));
        }
    }
}

void MoveGenerator::generate_rook_moves(const Position& pos, std::vector<Move>& moves) {
    Color us = pos.side_to_move();
    Bitboard rooks = pos.pieces(us, ROOK);
    
    while (rooks) {
        Square from = pop_lsb(rooks);
        Bitboard attacks = attacks_bb(ROOK, from, pos.pieces());
        Bitboard targets = attacks & ~pos.pieces(us);
        
        while (targets) {
            Square to = pop_lsb(targets);
            moves.push_back(Move(from, to));
        }
    }
}

void MoveGenerator::generate_queen_moves(const Position& pos, std::vector<Move>& moves) {
    Color us = pos.side_to_move();
    Bitboard queens = pos.pieces(us, QUEEN);
    
    while (queens) {
        Square from = pop_lsb(queens);
        Bitboard attacks = attacks_bb(BISHOP, from, pos.pieces()) | attacks_bb(ROOK, from, pos.pieces());
        Bitboard targets = attacks & ~pos.pieces(us);
        
        while (targets) {
            Square to = pop_lsb(targets);
            moves.push_back(Move(from, to));
        }
    }
}

void MoveGenerator::generate_king_moves(const Position& pos, std::vector<Move>& moves) {
    Color us = pos.side_to_move();
    Square from = pos.square<KING>(us);
    Bitboard attacks = attacks_bb(KING, from, 0);
    Bitboard targets = attacks & ~pos.pieces(us);
    
    while (targets) {
        Square to = pop_lsb(targets);
        moves.push_back(Move(from, to));
    }
}

void MoveGenerator::generate_castling_moves(const Position& pos, std::vector<Move>& moves) {
    Color us = pos.side_to_move();
    
    if (pos.can_castle(us == WHITE ? WHITE_OO : BLACK_OO)) {
        moves.push_back(Move::make<CASTLING>(pos.square<KING>(us), pos.castling_rook_square(us == WHITE ? WHITE_OO : BLACK_OO)));
    }
    if (pos.can_castle(us == WHITE ? WHITE_OOO : BLACK_OOO)) {
        moves.push_back(Move::make<CASTLING>(pos.square<KING>(us), pos.castling_rook_square(us == WHITE ? WHITE_OOO : BLACK_OOO)));
    }
}

void MoveGenerator::generate_en_passant_moves(const Position& pos, std::vector<Move>& moves) {
    if (pos.ep_square() != SQ_NONE) {
        Color us = pos.side_to_move();
        Color them = Color(us ^ 1);
        Bitboard attacks = pawn_attacks_bb(them, square_bb(pos.ep_square()));
        Bitboard our_pawns = pos.pieces(us, PAWN);
        
        Bitboard targets = attacks & our_pawns;
        while (targets) {
            Square from = pop_lsb(targets);
            moves.push_back(Move::make<EN_PASSANT>(from, pos.ep_square()));
        }
    }
}

bool MoveGenerator::is_legal_move(const Position& pos, Move move) {
    return pos.legal(move);
}

bool MoveGenerator::is_in_check(const Position& pos) {
    return pos.checkers();
}

bool MoveGenerator::is_square_attacked(const Position& pos, Square square, Color by_color) {
    return pos.attackers_to_exist(square, pos.pieces(), by_color);
}

Bitboard MoveGenerator::get_pawn_attacks(Square square, Color color) {
    return pawn_attacks_bb(color, square_bb(square));
}

Bitboard MoveGenerator::get_knight_attacks(Square square) {
    return attacks_bb(KNIGHT, square, 0);
}

Bitboard MoveGenerator::get_bishop_attacks(Square square, Bitboard occupied) {
    return attacks_bb(BISHOP, square, occupied);
}

Bitboard MoveGenerator::get_rook_attacks(Square square, Bitboard occupied) {
    return attacks_bb(ROOK, square, occupied);
}

Bitboard MoveGenerator::get_queen_attacks(Square square, Bitboard occupied) {
    return attacks_bb(BISHOP, square, occupied) | attacks_bb(ROOK, square, occupied);
}

Bitboard MoveGenerator::get_king_attacks(Square square) {
    return attacks_bb(KING, square, 0);
}

void MoveGenerator::init_magic_bitboards() {
    // This is now handled by Bitboards::init()
}

} // namespace Cerberus
