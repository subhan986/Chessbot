#include "evaluation/positional.h"

namespace Cerberus {

namespace Positional {

// Positional tables (simplified)
const int PAWN_POSITION_TABLE[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    50, 50, 50, 50, 50, 50, 50, 50,
    10, 10, 20, 30, 30, 20, 10, 10,
    5,  5, 10, 25, 25, 10,  5,  5,
    0,  0,  0, 20, 20,  0,  0,  0,
    5, -5,-10,  0,  0,-10, -5,  5,
    5, 10, 10,-20,-20, 10, 10,  5,
    0,  0,  0,  0,  0,  0,  0,  0
};

const int KNIGHT_POSITION_TABLE[64] = {
    -50,-40,-30,-30,-30,-30,-40,-50,
    -40,-20,  0,  0,  0,  0,-20,-40,
    -30,  0, 10, 15, 15, 10,  0,-30,
    -30,  5, 15, 20, 20, 15,  5,-30,
    -30,  0, 15, 20, 20, 15,  0,-30,
    -30,  5, 10, 15, 15, 10,  5,-30,
    -40,-20,  0,  5,  5,  0,-20,-40,
    -50,-40,-30,-30,-30,-30,-40,-50
};

const int BISHOP_POSITION_TABLE[64] = {
    -20,-10,-10,-10,-10,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5, 10, 10,  5,  0,-10,
    -10,  5,  5, 10, 10,  5,  5,-10,
    -10,  0, 10, 10, 10, 10,  0,-10,
    -10, 10, 10, 10, 10, 10, 10,-10,
    -10,  5,  0,  0,  0,  0,  5,-10,
    -20,-10,-10,-10,-10,-10,-10,-20
};

const int ROOK_POSITION_TABLE[64] = {
    0,  0,  0,  0,  0,  0,  0,  0,
    5, 10, 10, 10, 10, 10, 10,  5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    -5,  0,  0,  0,  0,  0,  0, -5,
    0,  0,  0,  5,  5,  0,  0,  0
};

const int QUEEN_POSITION_TABLE[64] = {
    -20,-10,-10, -5, -5,-10,-10,-20,
    -10,  0,  0,  0,  0,  0,  0,-10,
    -10,  0,  5,  5,  5,  5,  0,-10,
    -5,  0,  5,  5,  5,  5,  0, -5,
    0,  0,  5,  5,  5,  5,  0, -5,
    -10,  5,  5,  5,  5,  5,  0,-10,
    -10,  0,  5,  0,  0,  0,  0,-10,
    -20,-10,-10, -5, -5,-10,-10,-20
};

const int KING_POSITION_TABLE[64] = {
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -30,-40,-40,-50,-50,-40,-40,-30,
    -20,-30,-30,-40,-40,-30,-30,-20,
    -10,-20,-20,-20,-20,-20,-20,-10,
    20, 20,  0,  0,  0,  0, 20, 20,
    20, 30, 10,  0,  0, 10, 30, 20
};

// Positional evaluation
Score evaluate_positional_score(const Board& board) {
    Score score = 0;
    score += evaluate_piece_position(board, WHITE);
    score -= evaluate_piece_position(board, BLACK);
    return score;
}

Score evaluate_piece_position(const Board& board, Color color) {
    Score score = 0;
    
    // Evaluate each piece type
    for (PieceType type = PAWN; type <= KING; type = static_cast<PieceType>(type + 1)) {
        Bitboard pieces = board.get_piece_bitboard(color, type);
        while (pieces) {
            int square = pieces.pop_lsb();
            score += get_positional_bonus(type, square, color);
        }
    }
    
    return score;
}

// Get positional bonus for a piece on a square
int get_positional_bonus(PieceType type, int square, Color color) {
    int table_square = square;
    if (color == BLACK) {
        table_square = 63 - square; // Flip for black
    }
    
    switch (type) {
        case PAWN: return PAWN_POSITION_TABLE[table_square];
        case KNIGHT: return KNIGHT_POSITION_TABLE[table_square];
        case BISHOP: return BISHOP_POSITION_TABLE[table_square];
        case ROOK: return ROOK_POSITION_TABLE[table_square];
        case QUEEN: return QUEEN_POSITION_TABLE[table_square];
        case KING: return KING_POSITION_TABLE[table_square];
        default: return 0;
    }
}

// Endgame detection
bool is_endgame(const Board& board) {
    int piece_count = 0;
    
    // Count non-pawn pieces
    for (PieceType type = KNIGHT; type <= QUEEN; type = static_cast<PieceType>(type + 1)) {
        piece_count += board.get_piece_bitboard(WHITE, type).count();
        piece_count += board.get_piece_bitboard(BLACK, type).count();
    }
    
    return piece_count <= 6;
}

} // namespace Positional

} // namespace Cerberus 