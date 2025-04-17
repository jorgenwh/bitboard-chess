 #include "bitboard.h"

namespace chess {

Bitboard knight_attacks[64];
Bitboard king_attacks[64];
Bitboard pawn_attacks[2][64];

// Sliding attacks for bishops, rooks, and queens
Bitboard sliding_attacks(int sq, Bitboard occ, const int *deltas, int count) {
    Bitboard attacks = 0;
    for (int i = 0; i < count; ++i) {
        int dir = deltas[i];
        int s = sq;
        while (true) {
            int r = s / 8, f = s % 8;
            bool edge = false;
            if ((dir == 1 || dir == 9 || dir == -7) && f == 7) edge = true;
            else if ((dir == -1 || dir == 7 || dir == -9) && f == 0) edge = true;
            if (edge) break;
            s += dir;
            if (s < 0 || s > 63) break;
            attacks |= (1ULL << s);
            if (occ & (1ULL << s)) break;
        }
    }
    return attacks;
}

// Generate knight attack table
static void init_knight_attacks_table() {
    int knight_d[8][2] = {{2,1},{1,2},{-1,2},{-2,1},{-2,-1},{-1,-2},{1,-2},{2,-1}};
    for (int sq = 0; sq < 64; ++sq) {
        int r = sq / 8, f = sq % 8;
        Bitboard b = 0;
        for (int i = 0; i < 8; ++i) {
            int nr = r + knight_d[i][0], nf = f + knight_d[i][1];
            if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8)
                b |= (1ULL << (nr*8 + nf));
        }
        knight_attacks[sq] = b;
    }
}

// Generate king attack table
static void init_king_attacks_table() {
    int king_d[8][2] = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
    for (int sq = 0; sq < 64; ++sq) {
        int r = sq / 8, f = sq % 8;
        Bitboard b = 0;
        for (int i = 0; i < 8; ++i) {
            int nr = r + king_d[i][0], nf = f + king_d[i][1];
            if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8)
                b |= (1ULL << (nr*8 + nf));
        }
        king_attacks[sq] = b;
    }
}

// Generate pawn attack table for both colors
static void init_pawn_attacks_table() {
    for (int sq = 0; sq < 64; ++sq) {
        int r = sq / 8, f = sq % 8;
        Bitboard b = 0;
        if (r+1 < 8 && f-1 >= 0) b |= (1ULL << ((r+1)*8 + (f-1)));
        if (r+1 < 8 && f+1 < 8)  b |= (1ULL << ((r+1)*8 + (f+1)));
        pawn_attacks[WHITE][sq] = b;
        b = 0;
        if (r-1 >= 0 && f-1 >= 0) b |= (1ULL << ((r-1)*8 + (f-1)));
        if (r-1 >= 0 && f+1 < 8)  b |= (1ULL << ((r-1)*8 + (f+1)));
        pawn_attacks[BLACK][sq] = b;
    }
}

// Initialize precomputed attack tables
void init_attack_tables() {
    static bool initialized = false;
    if (initialized) return;
    initialized = true;
    init_knight_attacks_table();
    init_king_attacks_table();
    init_pawn_attacks_table();
}

} // namespace chess