#include "bitboard.h"

namespace Bitboards
{

// define constants for ranks
const Bitboard RANK_1 = 0x00000000000000FFULL;
const Bitboard RANK_2 = 0x000000000000FF00ULL;
const Bitboard RANK_3 = 0x0000000000FF0000ULL;
const Bitboard RANK_4 = 0x00000000FF000000ULL;
const Bitboard RANK_5 = 0x000000FF00000000ULL;
const Bitboard RANK_6 = 0x0000FF0000000000ULL;
const Bitboard RANK_7 = 0x00FF000000000000ULL;
const Bitboard RANK_8 = 0xFF00000000000000ULL;
const std::array<Bitboard, 8> RANKS = {
    RANK_1, RANK_2, RANK_3, RANK_4, RANK_5, RANK_6, RANK_7, RANK_8
};

// define constants for files
const Bitboard FILE_A = 0x0101010101010101ULL;
const Bitboard FILE_B = 0x0202020202020202ULL;
const Bitboard FILE_C = 0x0404040404040404ULL;
const Bitboard FILE_D = 0x0808080808080808ULL;
const Bitboard FILE_E = 0x1010101010101010ULL;
const Bitboard FILE_F = 0x2020202020202020ULL;
const Bitboard FILE_G = 0x4040404040404040ULL;
const Bitboard FILE_H = 0x8080808080808080ULL;
const std::array<Bitboard, 8> FILES = {
    FILE_A, FILE_B, FILE_C, FILE_D, FILE_E, FILE_F, FILE_G, FILE_H
};

// define constants for square colors
const Bitboard DARK_SQUARES = 0xAA55AA55AA55AA55ULL;
const Bitboard LIGHT_SQUARES = 0x55AA55AA55AA55AAULL;

// pre-computed attack tables
std::array<Bitboard, 64> KNIGHT_ATTACKS = {};
std::array<Bitboard, 64> KING_ATTACKS = {};
std::array<Bitboard, 64> PAWN_ATTACKS_WHITE = {};
std::array<Bitboard, 64> PAWN_ATTACKS_BLACK = {};

void init()
{
    // initialize knight attack table
    for (int sq = 0; sq < 64; sq++)
    {
        int r = sq / 8;
        int f = sq % 8;
        Bitboard b = 0ULL;

        // all 8 knight moves
        if (r > 0 && f > 1)
            b |= rank_file_bb(r - 1, f - 2);
        if (r > 0 && f < 6)
            b |= rank_file_bb(r - 1, f + 2);
        if (r > 1 && f > 0)
            b |= rank_file_bb(r - 2, f - 1);
        if (r > 1 && f < 7)
            b |= rank_file_bb(r - 2, f + 1);
        if (r < 7 && f > 1)
            b |= rank_file_bb(r + 1, f - 2);
        if (r < 7 && f < 6)
            b |= rank_file_bb(r + 1, f + 2);
        if (r < 6 && f > 0)
            b |= rank_file_bb(r + 2, f - 1);
        if (r < 6 && f < 7)
            b |= rank_file_bb(r + 2, f + 1);

        KNIGHT_ATTACKS[sq] = b;
    }

    // initialize king attack table
    for (int sq = 0; sq < 64; sq++)
    {
        int r = sq / 8;
        int f = sq % 8;
        Bitboard b = 0ULL;

        // all 8 king moves
        if (r > 0 && f > 0)
            b |= rank_file_bb(r - 1, f - 1);
        if (r > 0)
            b |= rank_file_bb(r - 1, f);
        if (r > 0 && f < 7)
            b |= rank_file_bb(r - 1, f + 1);
        if (f > 0)
            b |= rank_file_bb(r, f - 1);
        if (f < 7)
            b |= rank_file_bb(r, f + 1);
        if (r < 7 && f > 0)
            b |= rank_file_bb(r + 1, f - 1);
        if (r < 7)
            b |= rank_file_bb(r + 1, f);
        if (r < 7 && f < 7)
            b |= rank_file_bb(r + 1, f + 1);

        KING_ATTACKS[sq] = b;
    }

    // initialize pawn attack tables
    for (int sq = 0; sq < 64; sq++)
    {
        int r = sq / 8;
        int f = sq % 8;
        Bitboard whiteAttacks = 0ULL;
        Bitboard blackAttacks = 0ULL;

        // white pawns attack diagonally upward
        if (r < 7)
        {
            if (f > 0)
                whiteAttacks |= rank_file_bb(r + 1, f - 1);
            if (f < 7)
                whiteAttacks |= rank_file_bb(r + 1, f + 1);
        }

        // black pawns attack diagonally downward
        if (r > 0)
        {
            if (f > 0)
                blackAttacks |= rank_file_bb(r - 1, f - 1);
            if (f < 7)
                blackAttacks |= rank_file_bb(r - 1, f + 1);
        }

        PAWN_ATTACKS_WHITE[sq] = whiteAttacks;
        PAWN_ATTACKS_BLACK[sq] = blackAttacks;
    }
}

Bitboard square_bb(Square square)
{
    return 1ULL << square;
}

Bitboard rank_file_bb(int rank, int file)
{
    return 1ULL << (rank * 8 + file);
}

int popcount(Bitboard b)
{
    return std::popcount(b);
}

int lsb(Bitboard b)
{
    if (b == 0) return -1;
    return std::countr_zero(b);
}

int msb(Bitboard b)
{
    if (b == 0) return -1;
    return 63 - std::countl_zero(b);
}

Bitboard pop_lsb(Bitboard& b)
{
    Bitboard bb = b;
    b &= (b - 1);
    return bb & ~b;
}

Bitboard knight_attacks(Square square)
{
    return KNIGHT_ATTACKS[square];
}

Bitboard king_attacks(Square square)
{
    return KING_ATTACKS[square];
}

Bitboard pawn_attacks(Square square, bool isWhite)
{
    return isWhite ? PAWN_ATTACKS_WHITE[square] : PAWN_ATTACKS_BLACK[square];
}

std::string to_string(Bitboard b)
{
    std::string s = "";
    for (int rank = 7; rank >= 0; rank--)
    {
        for (int file = 0; file < 8; file++)
        {
            int square = rank * 8 + file;
            s += (b & (1ULL << square)) ? "1 " : "0 ";
        }
        s += "\n";
    }
    return s;
}

}
