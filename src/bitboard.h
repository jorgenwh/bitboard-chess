 #ifndef CHESS_BITBOARD_H
 #define CHESS_BITBOARD_H

 #include "types.h"

namespace chess {

// Find least significant bit index
inline int get_lsb_index(Bitboard b) {
    return __builtin_ctzll(b);
}

// Pop least significant bit and return it
inline Bitboard pop_lsb(Bitboard &b) {
    Bitboard lsb = b & -b;
    b &= b - 1;
    return lsb;
}

// Generate sliding attacks from square 'sq' in directions 'deltas'
Bitboard sliding_attacks(int sq, Bitboard occ, const int *deltas, int count);

// Initialize attack tables for knight, king, and pawn
void init_attack_tables();

// Precomputed attack tables
extern Bitboard knight_attacks[64];
extern Bitboard king_attacks[64];
extern Bitboard pawn_attacks[2][64];

} // namespace chess

#endif // CHESS_BITBOARD_H