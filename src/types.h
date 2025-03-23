#pragma once

#include <cstdint>
#include <string>

using Bitboard = uint64_t;

enum PieceType 
{
    NO_PIECE,
    PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING,
    PIECE_NB
};

enum Color 
{
    WHITE, BLACK,
    COLOR_NB
};

enum Square 
{
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    SQUARE_NB
};

enum CastlingRight 
{
    WHITE_KINGSIDE = 1,
    WHITE_QUEENSIDE = 2,
    BLACK_KINGSIDE = 4,
    BLACK_QUEENSIDE = 8
};

struct Move 
{
    Square from;
    Square to;
    PieceType promotion;

    Move() : from(SQUARE_NB), to(SQUARE_NB), promotion(NO_PIECE) {}
    Move(Square f, Square t) : from(f), to(t), promotion(NO_PIECE) {}
    Move(Square f, Square t, PieceType p) : from(f), to(t), promotion(p) {}

    bool operator==(const Move& other) const 
    {
        return from == other.from && to == other.to && promotion == other.promotion;
    }

    bool is_null() const 
    {
        return from == SQUARE_NB || to == SQUARE_NB;
    }

    std::string to_string() const;
};
