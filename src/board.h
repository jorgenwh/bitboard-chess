#pragma once

#include "types.h"
#include "bitboard.h"

#include <vector>
#include <string>
#include <sstream>
#include <cctype>
#include <iostream>

struct Board
{
    Board();
    Board(const std::string& fen);

    void reset();

    Bitboard pieces[Color::COLOR_NB][PieceType::PIECE_NB];
    Bitboard get_occupied() const;
    Bitboard get_occupied(Color color) const;
    Bitboard get_empty() const;

    Color side_to_move;
    Square en_passant_square;
    int castling_rights;

    bool from_fen(const std::string& fen);
    std::string get_fen() const;

    std::string to_string() const;
};
