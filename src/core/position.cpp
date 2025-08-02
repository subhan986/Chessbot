#include "core/position.h"
#include "core/bitboard.h"
#include "core/zobrist.h"
#include <iostream>
#include <sstream>

namespace Cerberus {

// Helper function for piece values
static int piece_value(Piece pc) {
    static const int values[6] = {100, 320, 330, 500, 900, 20000}; // P, N, B, R, Q, K
    return values[type_of(pc) - 1];
}

void Position::init() {
    // Initialize position-related tables
    // This would be called once at startup
}

Position& Position::set(const std::string& fenStr, bool isChess960, StateInfo* si) {
    // Clear the board
    std::memset(board, NO_PIECE, sizeof(board));
    std::memset(byTypeBB, 0, sizeof(byTypeBB));
    std::memset(byColorBB, 0, sizeof(byColorBB));
    std::memset(pieceCount, 0, sizeof(pieceCount));
    
    chess960 = isChess960;
    gamePly = 0;
    sideToMove = WHITE;
    
    std::istringstream iss(fenStr);
    std::string token;
    
    // Parse piece placement
    iss >> token;
    Square sq = SQ_A8;
    for (char c : token) {
        if (c == '/') {
            sq -= Direction(16); // Move down two ranks
        } else if (isdigit(c)) {
            sq += Direction((c - '0'));
        } else {
            Piece pc = NO_PIECE;
            switch (c) {
                case 'P': pc = W_PAWN; break;
                case 'N': pc = W_KNIGHT; break;
                case 'B': pc = W_BISHOP; break;
                case 'R': pc = W_ROOK; break;
                case 'Q': pc = W_QUEEN; break;
                case 'K': pc = W_KING; break;
                case 'p': pc = B_PAWN; break;
                case 'n': pc = B_KNIGHT; break;
                case 'b': pc = B_BISHOP; break;
                case 'r': pc = B_ROOK; break;
                case 'q': pc = B_QUEEN; break;
                case 'k': pc = B_KING; break;
            }
            if (pc != NO_PIECE) {
                put_piece(pc, sq);
                sq += EAST;
            }
        }
    }
    
    // Parse side to move
    iss >> token;
    sideToMove = (token == "w") ? WHITE : BLACK;
    
    // Parse castling rights (simplified)
    iss >> token;
    // For now, we'll skip detailed castling rights parsing
    
    // Parse en passant square
    iss >> token;
    if (token != "-") {
        // Parse en passant square
        // For now, we'll skip this
    }
    
    // Parse halfmove clock and fullmove number
    int halfmove, fullmove;
    iss >> halfmove >> fullmove;
    
    // Initialize state info
    if (si) {
        st = si;
        set_state();
    }
    
    return *this;
}

Position& Position::set(const std::string& code, Color c, StateInfo* si) {
    // Simplified position setup from code
    return set(code, false, si);
}

std::string Position::fen() const {
    std::ostringstream oss;
    
    // Piece placement
    for (Rank r = RANK_8; r >= RANK_1; --r) {
        int empty = 0;
        for (File f = FILE_A; f <= FILE_H; ++f) {
            Square s = make_square(f, r);
            Piece pc = piece_on(s);
            if (pc == NO_PIECE) {
                empty++;
            } else {
                if (empty > 0) {
                    oss << empty;
                    empty = 0;
                }
                char c = ' ';
                switch (type_of(pc)) {
                    case PAWN: c = (color_of(pc) == WHITE) ? 'P' : 'p'; break;
                    case KNIGHT: c = (color_of(pc) == WHITE) ? 'N' : 'n'; break;
                    case BISHOP: c = (color_of(pc) == WHITE) ? 'B' : 'b'; break;
                    case ROOK: c = (color_of(pc) == WHITE) ? 'R' : 'r'; break;
                    case QUEEN: c = (color_of(pc) == WHITE) ? 'Q' : 'q'; break;
                    case KING: c = (color_of(pc) == WHITE) ? 'K' : 'k'; break;
                }
                oss << c;
            }
        }
        if (empty > 0) {
            oss << empty;
        }
        if (r > RANK_1) {
            oss << '/';
        }
    }
    
    // Side to move
    oss << (sideToMove == WHITE ? " w " : " b ");
    
    // Castling rights (simplified)
    oss << "- ";
    
    // En passant square (simplified)
    oss << "- ";
    
    // Halfmove clock and fullmove number
    oss << "0 1";
    
    return oss.str();
}

void Position::set_state() const {
    // Initialize state info
    if (!st) return;
    
    // Initialize keys and material values
    st->materialKey = 0;
    st->pawnKey = 0;
    st->minorPieceKey = 0;
    st->nonPawnMaterial[WHITE] = st->nonPawnMaterial[BLACK] = 0;
    
    for (Square s = SQ_A1; s <= SQ_H8; ++s) {
        Piece pc = piece_on(s);
        if (pc != NO_PIECE) {
            // Update material key
            st->materialKey ^= Zobrist::piece_key(pc, s);
            
            // Update pawn key
            if (type_of(pc) == PAWN) {
                st->pawnKey ^= Zobrist::piece_key(pc, s);
            }
            
            // Update minor piece key
            if (type_of(pc) == KNIGHT || type_of(pc) == BISHOP) {
                st->minorPieceKey ^= Zobrist::piece_key(pc, s);
            }
            
            // Update non-pawn material
            if (type_of(pc) != PAWN && type_of(pc) != KING) {
                st->nonPawnMaterial[color_of(pc)] += piece_value(pc);
            }
        }
    }
    
    // Update side to move key
    if (sideToMove == BLACK) {
        st->materialKey ^= Zobrist::side_key();
    }
    
    // Initialize other state info
    st->castlingRights = 0;
    st->rule50 = 0;
    st->pliesFromNull = 0;
    st->epSquare = SQ_NONE;
    st->key = st->materialKey;
    st->checkersBB = 0;
    st->previous = nullptr;
    st->capturedPiece = NO_PIECE;
    st->repetition = 0;
    
    // Initialize check info
    set_check_info();
}

void Position::set_check_info() const {
    if (!st) return;
    
    // Simplified check info setup
    // In a full implementation, this would compute checkers, blockers, etc.
    st->checkersBB = 0;
    
    // Check if king is in check
    Color us = side_to_move();
    Square king_sq = square<KING>(us);
    
    // Check for attacks on king
    for (Color c = WHITE; c <= BLACK; ++c) {
        for (PieceType pt = PAWN; pt <= KING; ++pt) {
            Bitboard pieces = this->pieces(c, pt);
            while (pieces) {
                Square s = pop_lsb(pieces);
                Bitboard attacks = attacks_bb(pt, s, this->pieces());
                if (attacks & king_sq) {
                    st->checkersBB |= square_bb(s);
                }
            }
        }
    }
}

void Position::undo_move(Move m) {
    // Simplified move undoing
    // In a full implementation, this would restore the previous state
    sideToMove = Color(~sideToMove);
    gamePly--;
}

void Position::do_null_move(StateInfo& newSt, const TranspositionTable& tt) {
    // Simplified null move
    newSt.previous = st;
    st = &newSt;
    sideToMove = Color(~sideToMove);
    gamePly++;
}

void Position::undo_null_move() {
    // Simplified null move undoing
    if (st && st->previous) {
        st = st->previous;
        sideToMove = Color(~sideToMove);
        gamePly--;
    }
}

bool Position::see_ge(Move m, int threshold) const {
    // Simplified Static Exchange Evaluation
    // In a full implementation, this would compute the SEE value
    return true;
}

bool Position::legal(Move m) const {
    // Simplified legality check
    // In a full implementation, this would check if the move leaves the king in check
    return m.is_ok();
}

bool Position::pseudo_legal(const Move m) const {
    // Simplified pseudo-legality check
    return m.is_ok();
}

bool Position::gives_check(Move m) const {
    // Simplified check detection
    // In a full implementation, this would check if the move gives check
    return false;
}

bool Position::is_draw(int ply) const {
    // Simplified draw detection
    return false;
}

bool Position::is_repetition(int ply) const {
    // Simplified repetition detection
    return false;
}

bool Position::upcoming_repetition(int ply) const {
    // Simplified upcoming repetition detection
    return false;
}

bool Position::has_repeated() const {
    // Simplified repetition detection
    return false;
}

bool Position::pos_is_ok() const {
    // Simplified position consistency check
    return true;
}

void Position::flip() {
    // Simplified position flipping
    // In a full implementation, this would flip the board
}

StateInfo* Position::state() const {
    return st;
}

} // namespace Cerberus
