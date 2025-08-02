#include "core/board.h"
#include "core/move_generator.h"
#include "core/zobrist.h"
#include <iostream>
#include <sstream>

namespace Cerberus {

// Board constructor
Board::Board() {
    reset();
}

// Reset board to starting position
void Board::reset() {
    // Clear all squares
    for (int i = 0; i < 64; i++) {
        squares[i] = EMPTY;
    }
    
    // Clear bitboards
    for (int color = 0; color < 2; color++) {
        for (int piece = 0; piece < 7; piece++) {
            piece_bitboards[color][piece] = 0;
        }
    }
    all_pieces = 0;
    
    // Set up starting position
    set_piece(A1, WHITE_ROOK);
    set_piece(B1, WHITE_KNIGHT);
    set_piece(C1, WHITE_BISHOP);
    set_piece(D1, WHITE_QUEEN);
    set_piece(E1, WHITE_KING);
    set_piece(F1, WHITE_BISHOP);
    set_piece(G1, WHITE_KNIGHT);
    set_piece(H1, WHITE_ROOK);
    
    for (int file = 0; file < 8; file++) {
        set_piece(make_square(file, 1), WHITE_PAWN);
    }
    
    set_piece(A8, BLACK_ROOK);
    set_piece(B8, BLACK_KNIGHT);
    set_piece(C8, BLACK_BISHOP);
    set_piece(D8, BLACK_QUEEN);
    set_piece(E8, BLACK_KING);
    set_piece(F8, BLACK_BISHOP);
    set_piece(G8, BLACK_KNIGHT);
    set_piece(H8, BLACK_ROOK);
    
    for (int file = 0; file < 8; file++) {
        set_piece(make_square(file, 6), BLACK_PAWN);
    }
    
    // Game state
    side_to_move = WHITE;
    castling_rights = WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE;
    en_passant_square = -1;
    halfmove_clock = 0;
    fullmove_number = 1;
    
    // Clear history
    history.clear();
    
    // Update bitboards and zobrist key
    update_bitboards();
    update_zobrist_key();
}

// Set board from FEN string
bool Board::set_from_fen(const std::string& fen) {
    reset();
    
    std::istringstream iss(fen);
    std::string piece_placement, side_to_move_str, castling_str, en_passant_str, halfmove_str, fullmove_str;
    
    if (!(iss >> piece_placement >> side_to_move_str >> castling_str >> en_passant_str >> halfmove_str >> fullmove_str)) {
        return false;
    }
    
    // Parse piece placement
    int square = A8;
    for (char c : piece_placement) {
        if (c == '/') {
            square = A8 + (get_rank(square) - 1) * 8;
        } else if (c >= '1' && c <= '8') {
            square += (c - '0');
        } else {
            Piece piece = EMPTY;
            switch (c) {
                case 'P': piece = WHITE_PAWN; break;
                case 'N': piece = WHITE_KNIGHT; break;
                case 'B': piece = WHITE_BISHOP; break;
                case 'R': piece = WHITE_ROOK; break;
                case 'Q': piece = WHITE_QUEEN; break;
                case 'K': piece = WHITE_KING; break;
                case 'p': piece = BLACK_PAWN; break;
                case 'n': piece = BLACK_KNIGHT; break;
                case 'b': piece = BLACK_BISHOP; break;
                case 'r': piece = BLACK_ROOK; break;
                case 'q': piece = BLACK_QUEEN; break;
                case 'k': piece = BLACK_KING; break;
                default: return false;
            }
            set_piece(square, piece);
            square++;
        }
    }
    
    // Parse side to move
    side_to_move = (side_to_move_str == "w") ? WHITE : BLACK;
    
    // Parse castling rights
    castling_rights = 0;
    for (char c : castling_str) {
        switch (c) {
            case 'K': castling_rights |= WHITE_KINGSIDE; break;
            case 'Q': castling_rights |= WHITE_QUEENSIDE; break;
            case 'k': castling_rights |= BLACK_KINGSIDE; break;
            case 'q': castling_rights |= BLACK_QUEENSIDE; break;
        }
    }
    
    // Parse en passant square
    if (en_passant_str != "-") {
        if (en_passant_str.length() == 2) {
            int file = en_passant_str[0] - 'a';
            int rank = en_passant_str[1] - '1';
            en_passant_square = make_square(file, rank);
        }
    }
    
    // Parse move counters
    halfmove_clock = std::stoi(halfmove_str);
    fullmove_number = std::stoi(fullmove_str);
    
    update_bitboards();
    update_zobrist_key();
    return true;
}

// Get FEN string
std::string Board::get_fen() const {
    std::ostringstream oss;
    
    // Piece placement
    for (int rank = 7; rank >= 0; rank--) {
        int empty_count = 0;
        for (int file = 0; file < 8; file++) {
            int square = make_square(file, rank);
            Piece piece = get_piece(square);
            
            if (piece == EMPTY) {
                empty_count++;
            } else {
                if (empty_count > 0) {
                    oss << empty_count;
                    empty_count = 0;
                }
                
                char piece_char = ' ';
                switch (get_piece_type(piece)) {
                    case PAWN: piece_char = 'p'; break;
                    case KNIGHT: piece_char = 'n'; break;
                    case BISHOP: piece_char = 'b'; break;
                    case ROOK: piece_char = 'r'; break;
                    case QUEEN: piece_char = 'q'; break;
                    case KING: piece_char = 'k'; break;
                    default: break;
                }
                
                if (get_color(piece) == WHITE) {
                    piece_char = toupper(piece_char);
                }
                oss << piece_char;
            }
        }
        
        if (empty_count > 0) {
            oss << empty_count;
        }
        
        if (rank > 0) {
            oss << '/';
        }
    }
    
    oss << ' ' << (side_to_move == WHITE ? 'w' : 'b') << ' ';
    
    // Castling rights
    bool has_castling = false;
    if (castling_rights & WHITE_KINGSIDE) { oss << 'K'; has_castling = true; }
    if (castling_rights & WHITE_QUEENSIDE) { oss << 'Q'; has_castling = true; }
    if (castling_rights & BLACK_KINGSIDE) { oss << 'k'; has_castling = true; }
    if (castling_rights & BLACK_QUEENSIDE) { oss << 'q'; has_castling = true; }
    if (!has_castling) oss << '-';
    
    oss << ' ';
    
    // En passant square
    if (en_passant_square >= 0) {
        int file = get_file(en_passant_square);
        int rank = get_rank(en_passant_square);
        oss << static_cast<char>('a' + file) << (rank + 1);
    } else {
        oss << '-';
    }
    
    oss << ' ' << halfmove_clock << ' ' << fullmove_number;
    
    return oss.str();
}

// Board state accessors
Piece Board::get_piece(int square) const {
    if (square >= 0 && square < 64) {
        return squares[square];
    }
    return EMPTY;
}

void Board::set_piece(int square, Piece piece) {
    if (square >= 0 && square < 64) {
        squares[square] = piece;
    }
}

Bitboard Board::get_piece_bitboard(Color color, PieceType type) const {
    return piece_bitboards[color][type];
}

// Move making
bool Board::make_move(Move move) {
    if (!move.is_valid()) return false;
    
    save_state();
    
    int from = move.from();
    int to = move.to();
    Piece piece = get_piece(from);
    Piece captured = get_piece(to);
    
    // Remove piece from source
    set_piece(from, EMPTY);
    
    // Place piece on destination
    if (move.is_promotion()) {
        set_piece(to, make_piece(side_to_move, move.promotion()));
    } else {
        set_piece(to, piece);
    }
    
    // Handle special moves
    handle_special_moves(move);
    
    // Update game state
    side_to_move = (side_to_move == WHITE) ? BLACK : WHITE;
    if (side_to_move == WHITE) {
        fullmove_number++;
    }
    
    // Update halfmove clock
    if (get_piece_type(piece) == PAWN || captured != EMPTY) {
        halfmove_clock = 0;
    } else {
        halfmove_clock++;
    }
    
    update_bitboards();
    update_zobrist_key();
    
    return true;
}

void Board::unmake_move() {
    if (history.empty()) return;
    
    BoardState state = history.back();
    history.pop_back();
    
    restore_state(state);
}

bool Board::make_null_move() {
    save_state();
    side_to_move = (side_to_move == WHITE) ? BLACK : WHITE;
    en_passant_square = -1;
    update_zobrist_key();
    return true;
}

void Board::unmake_null_move() {
    unmake_move();
}

// Position analysis
bool Board::is_in_check() const {
    return is_in_check(side_to_move);
}

bool Board::is_in_check(Color color) const {
    // Find king
    int king_square = -1;
    Piece king_piece = make_piece(color, KING);
    
    for (int square = 0; square < 64; square++) {
        if (get_piece(square) == king_piece) {
            king_square = square;
            break;
        }
    }
    
    if (king_square == -1) return false;
    
    // Check if king square is attacked by opponent
    return MoveGenerator::is_square_attacked(*this, king_square, (color == WHITE) ? BLACK : WHITE);
}

bool Board::is_checkmate() const {
    if (!is_in_check()) return false;
    
    std::vector<Move> moves;
    MoveGenerator::generate_moves(*this, moves);
    return moves.empty();
}

bool Board::is_stalemate() const {
    if (is_in_check()) return false;
    
    std::vector<Move> moves;
    MoveGenerator::generate_moves(*this, moves);
    return moves.empty();
}

bool Board::is_draw() const {
    return is_stalemate() || halfmove_clock >= 50;
}

bool Board::is_legal_move(Move move) const {
    // Simplified implementation
    return move.is_valid();
}

// Utility functions
void Board::print_board() const {
    std::cout << "  +---+---+---+---+---+---+---+---+" << std::endl;
    for (int rank = 7; rank >= 0; rank--) {
        std::cout << (rank + 1) << " |";
        for (int file = 0; file < 8; file++) {
            int square = make_square(file, rank);
            Piece piece = get_piece(square);
            
            char piece_char = ' ';
            switch (get_piece_type(piece)) {
                case PAWN: piece_char = 'p'; break;
                case KNIGHT: piece_char = 'n'; break;
                case BISHOP: piece_char = 'b'; break;
                case ROOK: piece_char = 'r'; break;
                case QUEEN: piece_char = 'q'; break;
                case KING: piece_char = 'k'; break;
                default: piece_char = '.'; break;
            }
            
            if (get_color(piece) == WHITE) {
                piece_char = toupper(piece_char);
            }
            
            std::cout << " " << piece_char << " |";
        }
        std::cout << std::endl << "  +---+---+---+---+---+---+---+---+" << std::endl;
    }
    std::cout << "    a   b   c   d   e   f   g   h" << std::endl;
}

void Board::update_bitboards() {
    // Clear all bitboards
    for (int color = 0; color < 2; color++) {
        for (int piece = 0; piece < 7; piece++) {
            piece_bitboards[color][piece] = 0;
        }
    }
    all_pieces = 0;
    
    // Update bitboards from squares
    for (int square = 0; square < 64; square++) {
        Piece piece = get_piece(square);
        if (piece != EMPTY) {
            Color color = get_color(piece);
            PieceType type = get_piece_type(piece);
            piece_bitboards[color][type].set(square);
            all_pieces.set(square);
        }
    }
}

void Board::handle_special_moves(Move move) {
    // Handle castling
    if (move.is_castle()) {
        // Simplified castling implementation
    }
    
    // Handle en passant
    if (move.is_en_passant()) {
        // Simplified en passant implementation
    }
    
    // Update en passant square
    Piece piece = get_piece(move.to());
    if (get_piece_type(piece) == PAWN) {
        int from_rank = get_rank(move.from());
        int to_rank = get_rank(move.to());
        if (abs(to_rank - from_rank) == 2) {
            en_passant_square = (from_rank + to_rank) / 2 * 8 + get_file(move.from());
        } else {
            en_passant_square = -1;
        }
    } else {
        en_passant_square = -1;
    }
}

void Board::update_zobrist_key() {
    zobrist_key = Zobrist::compute_key(*this);
}

void Board::save_state() {
    BoardState state;
    state.zobrist_key = zobrist_key;
    state.castling_rights = castling_rights;
    state.en_passant_square = en_passant_square;
    state.halfmove_clock = halfmove_clock;
    state.fullmove_number = fullmove_number;
    state.last_move = Move();
    state.captured_piece = EMPTY;
    history.push_back(state);
}

void Board::restore_state(const BoardState& state) {
    zobrist_key = state.zobrist_key;
    castling_rights = state.castling_rights;
    en_passant_square = state.en_passant_square;
    halfmove_clock = state.halfmove_clock;
    fullmove_number = state.fullmove_number;
    side_to_move = (side_to_move == WHITE) ? BLACK : WHITE;
}

} // namespace Cerberus 