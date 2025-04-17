 #include "movegen.h"
 #include "bitboard.h"

namespace chess {

// Determine if square 'sq' is attacked by side 'attacker'
static bool is_square_attacked(const Position &pos, int sq, Color attacker) {
    // Pawn attacks
    if (attacker == WHITE) {
        if (pawn_attacks[BLACK][sq] & pos.pieces[WP]) return true;
    } else {
        if (pawn_attacks[WHITE][sq] & pos.pieces[BP]) return true;
    }
    // Knight attacks
    if (knight_attacks[sq] & pos.pieces[attacker == WHITE ? WN : BN]) return true;
    // King attacks
    if (king_attacks[sq] & pos.pieces[attacker == WHITE ? WK : BK]) return true;
    // Sliding attacks
    Bitboard occ = pos.occupancies[2];
    static const int bishop_dirs[4] = {9, 7, -9, -7};
    Bitboard diag_att = sliding_attacks(sq, occ, bishop_dirs, 4);
    Bitboard diag_mask = pos.pieces[attacker == WHITE ? WB : BB] |
                         pos.pieces[attacker == WHITE ? WQ : BQ];
    if (diag_att & diag_mask) return true;
    static const int rook_dirs[4] = {8, -8, 1, -1};
    Bitboard orth_att = sliding_attacks(sq, occ, rook_dirs, 4);
    Bitboard orth_mask = pos.pieces[attacker == WHITE ? WR : BR] |
                         pos.pieces[attacker == WHITE ? WQ : BQ];
    if (orth_att & orth_mask) return true;
    return false;
}

static void generate_pawn_moves(const Position &pos, Color side, Color opp,
    Bitboard own_occ, Bitboard opp_occ, Bitboard all_occ,
    std::vector<Move> &moves) {
    Bitboard pawns = pos.pieces[side == WHITE ? WP : BP];
    while (pawns) {
        Bitboard b = pop_lsb(pawns);
        int from = get_lsb_index(b);
        if (side == WHITE) {
            int to = from + 8;
            if (to < 64 && !(all_occ & (1ULL << to))) {
                int r = from / 8;
                if (r == 6) {
                    moves.push_back({from, to, WQ, false, false, false});
                    moves.push_back({from, to, WR, false, false, false});
                    moves.push_back({from, to, WB, false, false, false});
                    moves.push_back({from, to, WN, false, false, false});
                } else {
                    moves.push_back({from, to, NO_PIECE, false, false, false});
                    if (r == 1) {
                        int to2 = from + 16;
                        if (!(all_occ & (1ULL << to2)))
                            moves.push_back({from, to2, NO_PIECE, false, false, false});
                    }
                }
            }
            Bitboard attacks_bb = pawn_attacks[WHITE][from] & opp_occ;
            while (attacks_bb) {
                Bitboard l = pop_lsb(attacks_bb);
                int to = get_lsb_index(l);
                int r = from / 8;
                if (r == 6) {
                    moves.push_back({from, to, WQ, true, false, false});
                    moves.push_back({from, to, WR, true, false, false});
                    moves.push_back({from, to, WB, true, false, false});
                    moves.push_back({from, to, WN, true, false, false});
                } else {
                    moves.push_back({from, to, NO_PIECE, true, false, false});
                }
            }
            if (pos.en_passant >= 0) {
                Bitboard epb = (1ULL << pos.en_passant);
                if (pawn_attacks[WHITE][from] & epb)
                    moves.push_back({from, pos.en_passant, NO_PIECE, true, true, false});
            }
        } else {
            int to = from - 8;
            if (to >= 0 && !(all_occ & (1ULL << to))) {
                int r = from / 8;
                if (r == 1) {
                    moves.push_back({from, to, BQ, false, false, false});
                    moves.push_back({from, to, BR, false, false, false});
                    moves.push_back({from, to, BB, false, false, false});
                    moves.push_back({from, to, BN, false, false, false});
                } else {
                    moves.push_back({from, to, NO_PIECE, false, false, false});
                    if (r == 6) {
                        int to2 = from - 16;
                        if (!(all_occ & (1ULL << to2)))
                            moves.push_back({from, to2, NO_PIECE, false, false, false});
                    }
                }
            }
            Bitboard attacks_bb = pawn_attacks[BLACK][from] & opp_occ;
            while (attacks_bb) {
                Bitboard l = pop_lsb(attacks_bb);
                int to = get_lsb_index(l);
                int r = from / 8;
                if (r == 1) {
                    moves.push_back({from, to, BQ, true, false, false});
                    moves.push_back({from, to, BR, true, false, false});
                    moves.push_back({from, to, BB, true, false, false});
                    moves.push_back({from, to, BN, true, false, false});
                } else {
                    moves.push_back({from, to, NO_PIECE, true, false, false});
                }
            }
            if (pos.en_passant >= 0) {
                Bitboard epb = (1ULL << pos.en_passant);
                if (pawn_attacks[BLACK][from] & epb)
                    moves.push_back({from, pos.en_passant, NO_PIECE, true, true, false});
            }
        }
    }
}

static void generate_knight_moves(const Position &pos, Color side,
    Bitboard own_occ, Bitboard opp_occ,
    std::vector<Move> &moves) {
    Bitboard knights = pos.pieces[side == WHITE ? WN : BN];
    while (knights) {
        Bitboard b = pop_lsb(knights);
        int from = get_lsb_index(b);
        Bitboard attacks_bb = knight_attacks[from] & ~own_occ;
        while (attacks_bb) {
            Bitboard l = pop_lsb(attacks_bb);
            int to = get_lsb_index(l);
            bool cap = bool(opp_occ & l);
            moves.push_back({from, to, NO_PIECE, cap, false, false});
        }
    }
}

static void generate_bishop_moves(const Position &pos, Color side,
    Bitboard own_occ, Bitboard opp_occ, Bitboard all_occ,
    std::vector<Move> &moves) {
    static const int bishop_dirs[4] = {9, 7, -9, -7};
    Bitboard bishops = pos.pieces[side == WHITE ? WB : BB];
    while (bishops) {
        Bitboard b = pop_lsb(bishops);
        int from = get_lsb_index(b);
        Bitboard att = sliding_attacks(from, all_occ, bishop_dirs, 4) & ~own_occ;
        while (att) {
            Bitboard l = pop_lsb(att);
            int to = get_lsb_index(l);
            bool cap = bool(opp_occ & l);
            moves.push_back({from, to, NO_PIECE, cap, false, false});
        }
    }
}

static void generate_rook_moves(const Position &pos, Color side,
    Bitboard own_occ, Bitboard opp_occ, Bitboard all_occ,
    std::vector<Move> &moves) {
    static const int rook_dirs[4] = {8, -8, 1, -1};
    Bitboard rooks = pos.pieces[side == WHITE ? WR : BR];
    while (rooks) {
        Bitboard b = pop_lsb(rooks);
        int from = get_lsb_index(b);
        Bitboard att = sliding_attacks(from, all_occ, rook_dirs, 4) & ~own_occ;
        while (att) {
            Bitboard l = pop_lsb(att);
            int to = get_lsb_index(l);
            bool cap = bool(opp_occ & l);
            moves.push_back({from, to, NO_PIECE, cap, false, false});
        }
    }
}

static void generate_queen_moves(const Position &pos, Color side,
    Bitboard own_occ, Bitboard opp_occ, Bitboard all_occ,
    std::vector<Move> &moves) {
    static const int bishop_dirs[4] = {9, 7, -9, -7};
    static const int rook_dirs[4] = {8, -8, 1, -1};
    Bitboard queens = pos.pieces[side == WHITE ? WQ : BQ];
    while (queens) {
        Bitboard b = pop_lsb(queens);
        int from = get_lsb_index(b);
        Bitboard att = (sliding_attacks(from, all_occ, bishop_dirs, 4) |
                        sliding_attacks(from, all_occ, rook_dirs, 4)) & ~own_occ;
        while (att) {
            Bitboard l = pop_lsb(att);
            int to = get_lsb_index(l);
            bool cap = bool(opp_occ & l);
            moves.push_back({from, to, NO_PIECE, cap, false, false});
        }
    }
}

static void generate_king_moves(const Position &pos, Color side,
    Bitboard own_occ, Bitboard opp_occ,
    std::vector<Move> &moves) {
    Bitboard king_bb = pos.pieces[side == WHITE ? WK : BK];
    int from = get_lsb_index(king_bb);
    Bitboard att = king_attacks[from] & ~own_occ;
    while (att) {
        Bitboard l = pop_lsb(att);
        int to = get_lsb_index(l);
        bool cap = bool(opp_occ & l);
        moves.push_back({from, to, NO_PIECE, cap, false, false});
    }
}

static void generate_castling_moves(const Position &pos, Color side,
    Bitboard all_occ, std::vector<Move> &moves) {
    if (side == WHITE) {
        // King side
        if (pos.castle_rights[0] &&
            !(all_occ & ((1ULL<<5)|(1ULL<<6))) &&
            !is_square_attacked(pos, 4, BLACK) &&
            !is_square_attacked(pos, 5, BLACK) &&
            !is_square_attacked(pos, 6, BLACK)) {
            moves.push_back({4, 6, NO_PIECE, false, false, true});
        }
        // Queen side
        if (pos.castle_rights[1] &&
            !(all_occ & ((1ULL<<1)|(1ULL<<2)|(1ULL<<3))) &&
            !is_square_attacked(pos, 4, BLACK) &&
            !is_square_attacked(pos, 3, BLACK) &&
            !is_square_attacked(pos, 2, BLACK)) {
            moves.push_back({4, 2, NO_PIECE, false, false, true});
        }
    } else {
        // King side
        if (pos.castle_rights[2] &&
            !(all_occ & ((1ULL<<61)|(1ULL<<62))) &&
            !is_square_attacked(pos, 60, WHITE) &&
            !is_square_attacked(pos, 61, WHITE) &&
            !is_square_attacked(pos, 62, WHITE)) {
            moves.push_back({60, 62, NO_PIECE, false, false, true});
        }
        // Queen side
        if (pos.castle_rights[3] &&
            !(all_occ & ((1ULL<<57)|(1ULL<<58)|(1ULL<<59))) &&
            !is_square_attacked(pos, 60, WHITE) &&
            !is_square_attacked(pos, 59, WHITE) &&
            !is_square_attacked(pos, 58, WHITE)) {
            moves.push_back({60, 58, NO_PIECE, false, false, true});
        }
    }
}

void generate_legal_moves(const Position &pos, std::vector<Move> &moves) {
    init_attack_tables();
    moves.clear();
    std::vector<Move> pseudo;
    pseudo.reserve(256);
    Color side = pos.side_to_move;
    Color opp = side == WHITE ? BLACK : WHITE;
    Bitboard own_occ = pos.occupancies[side];
    Bitboard opp_occ = pos.occupancies[opp];
    Bitboard all_occ = pos.occupancies[2];
    // Generate pseudolegal moves
    generate_pawn_moves(pos, side, opp, own_occ, opp_occ, all_occ, pseudo);
    generate_knight_moves(pos, side, own_occ, opp_occ, pseudo);
    generate_bishop_moves(pos, side, own_occ, opp_occ, all_occ, pseudo);
    generate_rook_moves(pos, side, own_occ, opp_occ, all_occ, pseudo);
    generate_queen_moves(pos, side, own_occ, opp_occ, all_occ, pseudo);
    generate_king_moves(pos, side, own_occ, opp_occ, pseudo);
    generate_castling_moves(pos, side, all_occ, pseudo);
    // Filter legal moves
    moves.reserve(pseudo.size());
    for (const Move &m : pseudo) {
        Position new_pos = pos;
        make_move(new_pos, m);
        int king_sq = get_lsb_index(new_pos.pieces[side == WHITE ? WK : BK]);
        if (!is_square_attacked(new_pos, king_sq, new_pos.side_to_move)) {
            moves.push_back(m);
        }
    }
}

} // namespace chess