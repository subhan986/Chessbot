#pragma once
#include "types.h"
#include "bitboard.h"
#include "move.h"
#include <string>
#include <vector>
#include <memory>

namespace Cerberus {

// Forward declarations
class TranspositionTable;

// StateInfo struct stores information needed to restore a Position object to
// its previous state when we retract a move.
struct StateInfo {
    // Copied when making a move
    Key    materialKey;
    Key    pawnKey;
    Key    minorPieceKey;
    Key    nonPawnKey[COLOR_NB];
    Value  nonPawnMaterial[COLOR_NB];
    int    castlingRights;
    int    rule50;
    int    pliesFromNull;
    Square epSquare;

    // Not copied when making a move (will be recomputed anyhow)
    Key        key;
    Bitboard   checkersBB;
    StateInfo* previous;
    Bitboard   blockersForKing[COLOR_NB];
    Bitboard   pinners[COLOR_NB];
    Bitboard   checkSquares[PIECE_TYPE_NB];
    Piece      capturedPiece;
    int        repetition;
};

// A list to keep track of the position states along the setup moves
using StateListPtr = std::unique_ptr<std::vector<StateInfo>>;

// Position class stores information regarding the board representation as
// pieces, side to move, hash keys, castling info, etc.
class Position {
public:
    static void init();

    Position() = default;
    Position(const Position&) = delete;
    Position& operator=(const Position&) = delete;

    // FEN string input/output
    Position&   set(const std::string& fenStr, bool isChess960, StateInfo* si);
    Position&   set(const std::string& code, Color c, StateInfo* si);
    std::string fen() const;

    // Position representation
    Bitboard pieces() const;  // All pieces
    template<typename... PieceTypes>
    Bitboard pieces(PieceTypes... pts) const;
    Bitboard pieces(Color c) const;
    template<typename... PieceTypes>
    Bitboard pieces(Color c, PieceTypes... pts) const;
    Piece    piece_on(Square s) const;
    Square   ep_square() const;
    bool     empty(Square s) const;
    template<PieceType Pt>
    int count(Color c) const;
    template<PieceType Pt>
    int count() const;
    template<PieceType Pt>
    Square square(Color c) const;

    // Castling
    CastlingRights castling_rights(Color c) const;
    bool           can_castle(CastlingRights cr) const;
    bool           castling_impeded(CastlingRights cr) const;
    Square         castling_rook_square(CastlingRights cr) const;

    // Checking
    Bitboard checkers() const;
    Bitboard blockers_for_king(Color c) const;
    Bitboard check_squares(PieceType pt) const;
    Bitboard pinners(Color c) const;

    // Attacks to/from a given square
    Bitboard attackers_to(Square s) const;
    Bitboard attackers_to(Square s, Bitboard occupied) const;
    bool     attackers_to_exist(Square s, Bitboard occupied, Color c) const;
    void     update_slider_blockers(Color c) const;
    template<PieceType Pt>
    Bitboard attacks_by(Color c) const;

    // Properties of moves
    bool  legal(Move m) const;
    bool  pseudo_legal(const Move m) const;
    bool  capture(Move m) const;
    bool  capture_stage(Move m) const;
    bool  gives_check(Move m) const;
    Piece moved_piece(Move m) const;
    Piece captured_piece() const;

    // Doing and undoing moves
    void       do_move(Move m, StateInfo& newSt, const TranspositionTable* tt);
    void       undo_move(Move m);
    void       do_null_move(StateInfo& newSt, const TranspositionTable& tt);
    void       undo_null_move();

    // Static Exchange Evaluation
    bool see_ge(Move m, int threshold = 0) const;

    // Accessing hash keys
    Key key() const;
    Key material_key() const;
    Key pawn_key() const;
    Key minor_piece_key() const;
    Key non_pawn_key(Color c) const;

    // Position properties
    Color side_to_move() const;
    int   game_ply() const;
    bool  is_chess960() const;
    bool  is_draw(int ply) const;
    bool  is_repetition(int ply) const;
    bool  upcoming_repetition(int ply) const;
    bool  has_repeated() const;
    int   rule50_count() const;
    Value non_pawn_material(Color c) const;
    Value non_pawn_material() const;

    // Position consistency check, for debugging
    bool pos_is_ok() const;
    void flip();

    StateInfo* state() const;

    void put_piece(Piece pc, Square s);
    void remove_piece(Square s);

private:
    // Initialization helpers (used while setting up a position)
    void set_castling_right(Color c, Square rfrom);
    void set_state() const;
    void set_check_info() const;

    // Other helpers
    void move_piece(Square from, Square to);
    template<bool Do>
    void do_castling(Color             us,
                     Square            from,
                     Square&           to,
                     Square&           rfrom,
                     Square&           rto,
                     StateInfo* const  si = nullptr);
    template<bool AfterMove>
    Key adjust_key50(Key k) const;

    // Data members
    Piece      board[SQUARE_NB];
    Bitboard   byTypeBB[PIECE_TYPE_NB];
    Bitboard   byColorBB[COLOR_NB];
    int        pieceCount[PIECE_NB];
    int        castlingRightsMask[SQUARE_NB];
    Square     castlingRookSquare[CASTLING_RIGHT_NB];
    Bitboard   castlingPath[CASTLING_RIGHT_NB];
    StateInfo* st;
    int        gamePly;
    Color      sideToMove;
    bool       chess960;
};

// Inline functions
inline Color Position::side_to_move() const { return sideToMove; }

inline Piece Position::piece_on(Square s) const {
    assert(is_ok(s));
    return board[s];
}

inline bool Position::empty(Square s) const { return piece_on(s) == NO_PIECE; }

inline Piece Position::moved_piece(Move m) const { return piece_on(m.from_sq()); }

inline Bitboard Position::pieces() const { return byTypeBB[ALL_PIECES]; }

template<typename... PieceTypes>
inline Bitboard Position::pieces(PieceTypes... pts) const {
    return (... | byTypeBB[pts]);
}

inline Bitboard Position::pieces(Color c) const { return byColorBB[c]; }

template<typename... PieceTypes>
inline Bitboard Position::pieces(Color c, PieceTypes... pts) const {
    return pieces(c) & pieces(pts...);
}

template<PieceType Pt>
inline int Position::count(Color c) const {
    return pieceCount[make_piece(c, Pt)];
}

template<PieceType Pt>
inline int Position::count() const {
    return count<Pt>(WHITE) + count<Pt>(BLACK);
}

template<PieceType Pt>
inline Square Position::square(Color c) const {
    assert(count<Pt>(c) == 1);
    return lsb(pieces(c, Pt));
}

inline Square Position::ep_square() const { return st->epSquare; }

inline bool Position::can_castle(CastlingRights cr) const { return st->castlingRights & cr; }

inline CastlingRights Position::castling_rights(Color c) const {
    return CastlingRights(st->castlingRights & (c == WHITE ? WHITE_CASTLING : BLACK_CASTLING));
}

inline bool Position::castling_impeded(CastlingRights cr) const {
    return castlingPath[cr] & pieces();
}

inline Square Position::castling_rook_square(CastlingRights cr) const {
    return castlingRookSquare[cr];
}

inline Bitboard Position::attackers_to(Square s) const { return attackers_to(s, pieces()); }

template<PieceType Pt>
inline Bitboard Position::attacks_by(Color c) const {
    Bitboard b = pieces(c, Pt);
    Bitboard attacks = 0;
    while (b) {
        attacks |= attacks_bb(Pt, pop_lsb(b), pieces());
    }
    return attacks;
}

inline Bitboard Position::checkers() const { return st->checkersBB; }

inline Bitboard Position::blockers_for_king(Color c) const { return st->blockersForKing[c]; }

inline Bitboard Position::pinners(Color c) const { return st->pinners[c]; }

inline Bitboard Position::check_squares(PieceType pt) const { return st->checkSquares[pt]; }

inline Key Position::key() const { return adjust_key50<false>(st->key); }

template<bool AfterMove>
inline Key Position::adjust_key50(Key k) const {
    return k ^ (st->rule50 << 16);
}

inline Key Position::pawn_key() const { return st->pawnKey; }

inline Key Position::material_key() const { return st->materialKey; }

inline Key Position::minor_piece_key() const { return st->minorPieceKey; }

inline Key Position::non_pawn_key(Color c) const { return st->nonPawnKey[c]; }

inline Value Position::non_pawn_material(Color c) const { return st->nonPawnMaterial[c]; }

inline Value Position::non_pawn_material() const {
    return non_pawn_material(WHITE) + non_pawn_material(BLACK);
}

inline int Position::game_ply() const { return gamePly; }

inline int Position::rule50_count() const { return st->rule50; }

inline bool Position::is_chess960() const { return chess960; }

inline bool Position::capture(Move m) const {
    return !empty(m.to_sq()) || m.type_of() == EN_PASSANT;
}

inline bool Position::capture_stage(Move m) const {
    return capture(m) || m.type_of() == PROMOTION;
}

inline Piece Position::captured_piece() const { return st->capturedPiece; }

inline void Position::put_piece(Piece pc, Square s) {
    board[s] = pc;
    byTypeBB[type_of(pc)] |= s;
    byTypeBB[ALL_PIECES] |= s;
    byColorBB[color_of(pc)] |= s;
    pieceCount[pc]++;
}

inline void Position::remove_piece(Square s) {
    Piece pc = board[s];
    board[s] = NO_PIECE;
    byTypeBB[type_of(pc)] ^= s;
    byTypeBB[ALL_PIECES] ^= s;
    byColorBB[color_of(pc)] ^= s;
    pieceCount[pc]--;
}

inline void Position::move_piece(Square from, Square to) {
    Piece pc = board[from];
    board[from] = NO_PIECE;
    board[to] = pc;
    byTypeBB[type_of(pc)] ^= from | to;
    byTypeBB[ALL_PIECES] ^= from | to;
    byColorBB[color_of(pc)] ^= from | to;
}

inline void Position::do_move(Move m, StateInfo& newSt, const TranspositionTable* tt = nullptr) {
    // Simplified move making - in full implementation this would be much more complex
    Square from = m.from_sq();
    Square to = m.to_sq();
    Piece pc = piece_on(from);
    
    // Store captured piece
    newSt.capturedPiece = piece_on(to);
    
    // Move piece
    move_piece(from, to);
    
    // Handle special moves (promotion, en passant, castling)
    if (m.type_of() == PROMOTION) {
        remove_piece(to);
        put_piece(make_piece(side_to_move(), m.promotion_type()), to);
    }
    
    // Update side to move
    sideToMove = (Color) !(sideToMove);
    gamePly++;
}

} // namespace Cerberus
