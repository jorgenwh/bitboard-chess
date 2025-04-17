 #ifndef CHESS_POSITION_H
 #define CHESS_POSITION_H

#include "types.h"
#include <array>
#include <string>

namespace chess {

struct Position {
    Bitboard pieces[12];
    Bitboard occupancies[3];
    Color side_to_move;
    int en_passant;
    std::array<bool, 4> castle_rights;
    int halfmove_clock;
    int fullmove_clock;
};

// Initialize starting position
void init_position(Position &pos);
// Set position from FEN string; returns false on invalid FEN
bool set_fen(Position &pos, const std::string &fen);
// Get FEN string from position
std::string get_fen(const Position &pos);
// Get ASCII diagram of the position
std::string position_to_string(const Position &pos);

// Make move and update position state
void make_move(Position &pos, const Move &m);

} // namespace chess

#endif // CHESS_POSITION_H