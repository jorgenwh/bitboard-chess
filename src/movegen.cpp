#include "movegen.h"
#include "board.h"
#include "bitboard.h"

namespace MoveGen
{

// main move generation functions
std::vector<Move> generate_legal_moves(const Board& board)
{
    // generate pseudo-legal moves
    std::vector<Move> moves = generate_pseudo_legal_moves(board);

    // filter out illegal moves (those that leave king in check)
    // this will be implemented later

    return moves;
}

std::vector<Move> generate_pseudo_legal_moves(const Board& board)
{
    std::vector<Move> moves(48);

    generate_pawn_moves(board, moves);
    generate_knight_moves(board, moves);
    generate_bishop_moves(board, moves);
    generate_rook_moves(board, moves);
    generate_queen_moves(board, moves);
    generate_king_moves(board, moves);

    return moves;
}

void generate_pawn_single_pushes(const Board& board, std::vector<Move>& moves)
{
    Color us = board.side_to_move;
    Color them = (us == WHITE) ? BLACK : WHITE;

    Bitboard pawns = board.pieces[us][PAWN];
    Bitboard empty = board.get_empty();

    Bitboard single_push_targets;
    if (us == WHITE)
    {
        single_push_targets = (pawns << 8) & empty;
    }
    else
    {
        single_push_targets = (pawns >> 8) & empty;
    }

    while (single_push_targets)
    {
        Square to = static_cast<Square>(Bitboards::pop_lsb(single_push_targets));
        Square from = static_cast<Square>(to + (us == WHITE ? -8 : 8));

        if ((us == WHITE && to >= A8) || (us == BLACK && to <= H1))
        {
            moves.emplace_back(from, to, KNIGHT);
            moves.emplace_back(from, to, BISHOP);
            moves.emplace_back(from, to, ROOK);
            moves.emplace_back(from, to, QUEEN);
        }
        else
        {
            moves.emplace_back(from, to);
        }
    }
}

void generate_pawn_double_pushes(const Board& board, std::vector<Move>& moves)
{
    Color us = board.side_to_move;
    Color them = (us == WHITE) ? BLACK : WHITE;

    Bitboard pawns = board.pieces[us][PAWN];
    Bitboard empty = board.get_empty();

    Bitboard double_push_candidates;
    Bitboard double_push_targets;

    if (us == WHITE)
    {
        double_push_candidates = pawns & Bitboards::RANK_2;
        Bitboard single_push = (double_push_candidates << 8) & empty;
        double_push_targets = (single_push << 8) & empty;
    }
    else
    {
        double_push_candidates = pawns & Bitboards::RANK_7;
        Bitboard single_push = (double_push_candidates >> 8) & empty;
        double_push_targets = (single_push >> 8) & empty;
    }

    while (double_push_targets)
    {
        Square to = static_cast<Square>(Bitboards::pop_lsb(double_push_targets));
        Square from = static_cast<Square>(to + (us == WHITE ? -16 : 16));

        moves.emplace_back(from, to);
    }
}

void generate_pawn_captures(const Board& board, std::vector<Move>& moves)
{
    Color us = board.side_to_move;
    Color them = (us == WHITE) ? BLACK : WHITE;

    Bitboard pawns = board.pieces[us][PAWN];
    Bitboard their_pieces = board.get_occupied(them);

    Bitboard captures_left, captures_right;

    if (us == WHITE)
    {
        // left and right captures for white should never be on file A and H respectively
        captures_left = (pawns << 7) & their_pieces & ~Bitboards::FILE_H;
        captures_right = (pawns << 9) & their_pieces & ~Bitboards::FILE_A;
    }
    else
    {
        // left and right captures for black should never be on file H and A respectively
        captures_left = (pawns >> 9) & their_pieces & ~Bitboards::FILE_H;
        captures_right = (pawns >> 7) & their_pieces & ~Bitboards::FILE_A;
    }

    while (captures_left)
    {
        Square to = static_cast<Square>(Bitboards::pop_lsb(captures_left));
        Square from = static_cast<Square>(to + (us == WHITE ? -7 : 9));

        if ((us == WHITE && to >= A8) || (us == BLACK && to <= H1))
        {
            moves.emplace_back(from, to, KNIGHT);
            moves.emplace_back(from, to, BISHOP);
            moves.emplace_back(from, to, ROOK);
            moves.emplace_back(from, to, QUEEN);
        }
        else
        {
            moves.emplace_back(from, to);
        }
    }

    while (captures_right)
    {
        Square to = static_cast<Square>(Bitboards::pop_lsb(captures_right));
        Square from = static_cast<Square>(to + (us == WHITE ? -9 : 7));

        if ((us == WHITE && to >= A8) || (us == BLACK && to <= H1))
        {
            moves.emplace_back(from, to, KNIGHT);
            moves.emplace_back(from, to, BISHOP);
            moves.emplace_back(from, to, ROOK);
            moves.emplace_back(from, to, QUEEN);
        }
        else
        {
            moves.emplace_back(from, to);
        }
    }
}

void generate_en_passant_captures(const Board& board, std::vector<Move>& moves)
{
    if (board.en_passant_square == SQUARE_NB)
    {
        return;
    }

    Color us = board.side_to_move;
    Square ep_square = board.en_passant_square;
    Bitboard eq_sq_bb = Bitboards::square_bb(ep_square);
    Bitboard pawns = board.pieces[us][PAWN];

    Bitboard ep_capturers;
    // pawns that can capture en passant are on the rank above or below the en passant square
    Bitboard ep_sq_neighbors = (1ULL << (ep_square - 1)) | (1ULL << (ep_square + 1));
    Bitboard eq_sq_neighbors // TODO

    if (us == WHITE)
    {
        // white pawns that can capture en passant are on rank 5
        ep_capturers = pawns & ep_sq_neighbors & Bitboards::RANK_5;
    }
    else
    {
        // black pawns that can capture en passant are on rank 4
        ep_capturers = pawns & ep_sq_neighbors & Bitboards::RANK_4;
    }

    while (ep_capturers)
    {
        Square from = static_cast<Square>(Bitboards::pop_lsb(ep_capturers));
        Square to = ep_square;

        moves.emplace_back(from, to);
    }
}

// main pawn move generation function
void generate_pawn_moves(const Board& board, std::vector<Move>& moves) 
{
    generate_pawn_single_pushes(board, moves);
    generate_pawn_double_pushes(board, moves);
    generate_pawn_captures(board, moves);
    generate_en_passant_captures(board, moves);
}

// placeholder for knight move generation
void generate_knight_moves(const Board& board, std::vector<Move>& moves) 
{
    // will be implemented next
}

// placeholder for bishop move generation
void generate_bishop_moves(const Board& board, std::vector<Move>& moves) 
{
    // will be implemented later
}

// placeholder for rook move generation
void generate_rook_moves(const Board& board, std::vector<Move>& moves) 
{
    // will be implemented later
}

// placeholder for queen move generation
void generate_queen_moves(const Board& board, std::vector<Move>& moves) 
{
    // will be implemented later
}

// placeholder for king move generation
void generate_king_moves(const Board& board, std::vector<Move>& moves) 
{
    // will be implemented later
}

} // namespace MoveGen
