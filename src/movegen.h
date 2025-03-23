#pragma once

#include "types.h"
#include "board.h"

#include <vector>

namespace MoveGen
{

std::vector<Move> generate_legal_moves(const Board& board);
std::vector<Move> generate_pseudo_legal_moves(const Board& board);

void generate_pawn_moves(const Board& board, std::vector<Move>& moves);
void generate_knight_moves(const Board& board, std::vector<Move>& moves);
void generate_bishop_moves(const Board& board, std::vector<Move>& moves);
void generate_rook_moves(const Board& board, std::vector<Move>& moves);
void generate_queen_moves(const Board& board, std::vector<Move>& moves);
void generate_king_moves(const Board& board, std::vector<Move>& moves);

} // namespace MoveGen
