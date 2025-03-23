#pragma once

#include <iostream>
#include <string>
#include <array>
#include <bit>

#include "types.h"

namespace Bitboards 
{

// constants for ranks and files
extern const Bitboard RANK_1;
extern const Bitboard RANK_2;
extern const Bitboard RANK_3;
extern const Bitboard RANK_4;
extern const Bitboard RANK_5;
extern const Bitboard RANK_6;
extern const Bitboard RANK_7;
extern const Bitboard RANK_8;
extern const std::array<Bitboard, 8> RANKS;

extern const Bitboard FILE_A;
extern const Bitboard FILE_B;
extern const Bitboard FILE_C;
extern const Bitboard FILE_D;
extern const Bitboard FILE_E;
extern const Bitboard FILE_F;
extern const Bitboard FILE_G;
extern const Bitboard FILE_H;
extern const std::array<Bitboard, 8> FILES;

// common masks
extern const Bitboard DARK_SQUARES;
extern const Bitboard LIGHT_SQUARES;

// precomputed attack tables
extern std::array<Bitboard, 64> KNIGHT_ATTACKS;
extern std::array<Bitboard, 64> KING_ATTACKS;
extern std::array<Bitboard, 64> PAWN_ATTACKS_WHITE;
extern std::array<Bitboard, 64> PAWN_ATTACKS_BLACK;

void init();

Bitboard square_bb(Square square);
Bitboard rank_file_bb(int rank, int file);

int popcount(Bitboard b);
int lsb(Bitboard b);
int msb(Bitboard b);
Bitboard pop_lsb(Bitboard& b);

Bitboard knight_attacks(Square square);
Bitboard king_attacks(Square square);
Bitboard pawn_attacks(Square square, bool isWhite);

std::string to_string(Bitboard b);

} // namespace Bitboards
