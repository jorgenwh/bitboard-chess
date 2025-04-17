 #include "board.h"
 #include <cstring>

 namespace chess {

 static Bitboard knightAttacks[64];
 static Bitboard kingAttacks[64];
 static Bitboard pawnAttacksArr[2][64];
 static bool tablesInitialized = false;

 static inline int getLSBIndex(Bitboard b) {
     return __builtin_ctzll(b);
 }

 static inline Bitboard popLSB(Bitboard& b) {
     Bitboard lsb = b & -b;
     b &= b - 1;
     return lsb;
 }

 static Bitboard slidingAttacks(int sq, Bitboard occ, const int* deltas, int count) {
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

 static bool isSquareAttacked(const Position& pos, int sq, Color attacker) {
    // pawn attacks: check pawns that could capture to 'sq'
    if (attacker == WHITE) {
        // white pawns attack upward: they must be on squares that are one row below target
        if ((pawnAttacksArr[BLACK][sq] & pos.pieces[WP]) != 0) return true;
    } else {
        // black pawns attack downward: they must be on squares one row above target
        if ((pawnAttacksArr[WHITE][sq] & pos.pieces[BP]) != 0) return true;
    }
     // knight attacks
     if (knightAttacks[sq] & pos.pieces[attacker == WHITE ? WN : BN]) return true;
     // king attacks
     if (kingAttacks[sq] & pos.pieces[attacker == WHITE ? WK : BK]) return true;
     Bitboard occ = pos.occupancies[2];
     // bishop/queen (diagonals)
     const int bishopDirs[4] = {9, 7, -9, -7};
     for (int i = 0; i < 4; ++i) {
         int dir = bishopDirs[i];
         int s = sq;
         while (true) {
             int r = s / 8, f = s % 8;
             bool edge = false;
             if ((dir == 9 && (f == 7 || r == 7)) ||
                 (dir == 7 && (f == 0 || r == 7)) ||
                 (dir == -9 && (f == 0 || r == 0)) ||
                 (dir == -7 && (f == 7 || r == 0))) edge = true;
             if (edge) break;
             s += dir;
             if (s < 0 || s > 63) break;
             Bitboard b = (1ULL << s);
             if (occ & b) {
                 if (pos.pieces[attacker == WHITE ? WB : BB] & b) return true;
                 if (pos.pieces[attacker == WHITE ? WQ : BQ] & b) return true;
                 break;
             }
         }
     }
     // rook/queen (orthogonal)
     const int rookDirs[4] = {8, -8, 1, -1};
     for (int i = 0; i < 4; ++i) {
         int dir = rookDirs[i];
         int s = sq;
         while (true) {
             int r = s / 8, f = s % 8;
             bool edge = false;
             if ((dir == 1 && f == 7) || (dir == -1 && f == 0)) edge = true;
             if (edge) break;
             s += dir;
             if (s < 0 || s > 63) break;
             Bitboard b = (1ULL << s);
             if (occ & b) {
                 if (pos.pieces[attacker == WHITE ? WR : BR] & b) return true;
                 if (pos.pieces[attacker == WHITE ? WQ : BQ] & b) return true;
                 break;
             }
         }
     }
     return false;
 }

 static void initAttackTables() {
     for (int sq = 0; sq < 64; ++sq) {
         int r = sq / 8, f = sq % 8;
         Bitboard b = 0;
         int knightD[8][2] = {{2,1},{1,2},{-1,2},{-2,1},{-2,-1},{-1,-2},{1,-2},{2,-1}};
         for (int i = 0; i < 8; ++i) {
             int nr = r + knightD[i][0], nf = f + knightD[i][1];
             if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8)
                 b |= (1ULL << (nr*8 + nf));
         }
         knightAttacks[sq] = b;
         b = 0;
         int kingD[8][2] = {{1,0},{1,1},{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1}};
         for (int i = 0; i < 8; ++i) {
             int nr = r + kingD[i][0], nf = f + kingD[i][1];
             if (nr >= 0 && nr < 8 && nf >= 0 && nf < 8)
                 b |= (1ULL << (nr*8 + nf));
         }
         kingAttacks[sq] = b;
         b = 0;
         if (r+1 < 8 && f-1 >= 0) b |= (1ULL << ((r+1)*8 + (f-1)));
         if (r+1 < 8 && f+1 < 8) b |= (1ULL << ((r+1)*8 + (f+1)));
         pawnAttacksArr[WHITE][sq] = b;
         b = 0;
         if (r-1 >= 0 && f-1 >= 0) b |= (1ULL << ((r-1)*8 + (f-1)));
         if (r-1 >= 0 && f+1 < 8) b |= (1ULL << ((r-1)*8 + (f+1)));
         pawnAttacksArr[BLACK][sq] = b;
     }
 }

 void initPosition(Position& pos) {
     if (!tablesInitialized) {
         initAttackTables();
         tablesInitialized = true;
     }
     std::memset(pos.pieces, 0, sizeof(pos.pieces));
     pos.pieces[WP] = 0x000000000000FF00ULL;
     pos.pieces[WN] = (1ULL<<1)|(1ULL<<6);
     pos.pieces[WB] = (1ULL<<2)|(1ULL<<5);
     pos.pieces[WR] = (1ULL<<0)|(1ULL<<7);
     pos.pieces[WQ] = (1ULL<<3);
     pos.pieces[WK] = (1ULL<<4);
     pos.pieces[BP] = 0x00FF000000000000ULL;
     pos.pieces[BN] = (1ULL<<57)|(1ULL<<62);
     pos.pieces[BB] = (1ULL<<58)|(1ULL<<61);
     pos.pieces[BR] = (1ULL<<56)|(1ULL<<63);
     pos.pieces[BQ] = (1ULL<<59);
     pos.pieces[BK] = (1ULL<<60);
     pos.occupancies[WHITE] = 0;
     pos.occupancies[BLACK] = 0;
     for (int i = 0; i < 6; ++i) pos.occupancies[WHITE] |= pos.pieces[i];
     for (int i = 6; i < 12; ++i) pos.occupancies[BLACK] |= pos.pieces[i];
     pos.occupancies[2] = pos.occupancies[WHITE] | pos.occupancies[BLACK];
     pos.sideToMove = WHITE;
     pos.enPassant = -1;
     pos.castleRights[0] = pos.castleRights[1] = pos.castleRights[2] = pos.castleRights[3] = true;
 }

 void generateLegalMoves(const Position& pos, std::vector<Move>& moves) {
     if (!tablesInitialized) {
         initAttackTables();
         tablesInitialized = true;
     }
     moves.clear();
     std::vector<Move> pseudo;
     pseudo.reserve(256);
     Color side = pos.sideToMove;
     Color opp = side==WHITE?BLACK:WHITE;
     Bitboard ownOcc = pos.occupancies[side];
     Bitboard oppOcc = pos.occupancies[opp];
     Bitboard allOcc = pos.occupancies[2];

     // Pawn moves
     Bitboard pawns = pos.pieces[side==WHITE?WP:BP];
     while (pawns) {
         Bitboard bit = popLSB(pawns);
         int from = getLSBIndex(bit);
         if (side == WHITE) {
             int to = from + 8;
             if (to < 64 && !(allOcc & (1ULL<<to))) {
                 int r = from/8;
                 if (r == 6) {
                     pseudo.push_back({from,to,WQ,false,false,false});
                     pseudo.push_back({from,to,WR,false,false,false});
                     pseudo.push_back({from,to,WB,false,false,false});
                     pseudo.push_back({from,to,WN,false,false,false});
                 } else {
                     pseudo.push_back({from,to,NO_PIECE,false,false,false});
                     if (r == 1) {
                         int to2 = from + 16;
                         if (!(allOcc & (1ULL<<to2)))
                             pseudo.push_back({from,to2,NO_PIECE,false,false,false});
                     }
                 }
             }
             Bitboard attacks = pawnAttacksArr[WHITE][from] & oppOcc;
             while (attacks) {
                 Bitboard b2 = popLSB(attacks);
                 int to = getLSBIndex(b2);
                 int r = from/8;
                 if (r == 6) {
                     pseudo.push_back({from,to,WQ,true,false,false});
                     pseudo.push_back({from,to,WR,true,false,false});
                     pseudo.push_back({from,to,WB,true,false,false});
                     pseudo.push_back({from,to,WN,true,false,false});
                 } else {
                     pseudo.push_back({from,to,NO_PIECE,true,false,false});
                 }
             }
                if (pos.enPassant >= 0) {
                    Bitboard epb = (1ULL<<pos.enPassant);
                    if (pawnAttacksArr[WHITE][from] & epb)
                        pseudo.push_back({from,pos.enPassant,NO_PIECE,true,true,false});
                }
         } else {
             int to = from - 8;
             if (to >= 0 && !(allOcc & (1ULL<<to))) {
                 int r = from/8;
                 if (r == 1) {
                     pseudo.push_back({from,to,BQ,false,false,false});
                     pseudo.push_back({from,to,BR,false,false,false});
                     pseudo.push_back({from,to,BB,false,false,false});
                     pseudo.push_back({from,to,BN,false,false,false});
                 } else {
                     pseudo.push_back({from,to,NO_PIECE,false,false,false});
                     if (r == 6) {
                         int to2 = from - 16;
                         if (!(allOcc & (1ULL<<to2)))
                             pseudo.push_back({from,to2,NO_PIECE,false,false,false});
                     }
                 }
             }
             Bitboard attacks = pawnAttacksArr[BLACK][from] & oppOcc;
             while (attacks) {
                 Bitboard b2 = popLSB(attacks);
                 int to = getLSBIndex(b2);
                 int r = from/8;
                 if (r == 1) {
                     pseudo.push_back({from,to,BQ,true,false,false});
                     pseudo.push_back({from,to,BR,true,false,false});
                     pseudo.push_back({from,to,BB,true,false,false});
                     pseudo.push_back({from,to,BN,true,false,false});
                 } else {
                     pseudo.push_back({from,to,NO_PIECE,true,false,false});
                 }
             }
                if (pos.enPassant >= 0) {
                    Bitboard epb = (1ULL<<pos.enPassant);
                    if (pawnAttacksArr[BLACK][from] & epb)
                        pseudo.push_back({from,pos.enPassant,NO_PIECE,true,true,false});
                }
         }
     }

     // Knight moves
     Bitboard knights = pos.pieces[side==WHITE?WN:BN];
     while (knights) {
         Bitboard bit = popLSB(knights);
         int from = getLSBIndex(bit);
         Bitboard attacks = knightAttacks[from] & ~ownOcc;
         while (attacks) {
             Bitboard b2 = popLSB(attacks);
             int to = getLSBIndex(b2);
             bool cap = bool(oppOcc & (1ULL<<to));
             pseudo.push_back({from,to,NO_PIECE,cap,false,false});
         }
     }

     // Bishop moves
     const int bishopDirsArr[4] = {9,7,-9,-7};
     Bitboard bishops = pos.pieces[side==WHITE?WB:BB];
     while (bishops) {
         Bitboard bit = popLSB(bishops);
         int from = getLSBIndex(bit);
         Bitboard attacks = slidingAttacks(from, allOcc, bishopDirsArr,4)&~ownOcc;
         while (attacks) {
             Bitboard b2 = popLSB(attacks);
             int to = getLSBIndex(b2);
             bool cap = bool(oppOcc&(1ULL<<to));
             pseudo.push_back({from,to,NO_PIECE,cap,false,false});
         }
     }

     // Rook moves
     const int rookDirsArr[4] = {8,-8,1,-1};
     Bitboard rooks = pos.pieces[side==WHITE?WR:BR];
     while (rooks) {
         Bitboard bit = popLSB(rooks);
         int from = getLSBIndex(bit);
         Bitboard attacks = slidingAttacks(from, allOcc, rookDirsArr,4)&~ownOcc;
         while (attacks) {
             Bitboard b2 = popLSB(attacks);
             int to = getLSBIndex(b2);
             bool cap = bool(oppOcc&(1ULL<<to));
             pseudo.push_back({from,to,NO_PIECE,cap,false,false});
         }
     }

     // Queen moves
     Bitboard queens = pos.pieces[side==WHITE?WQ:BQ];
     while (queens) {
         Bitboard bit = popLSB(queens);
         int from = getLSBIndex(bit);
         Bitboard attB = slidingAttacks(from, allOcc, bishopDirsArr,4);
         Bitboard attR = slidingAttacks(from, allOcc, rookDirsArr,4);
         Bitboard attacks = (attB|attR)&~ownOcc;
         while (attacks) {
             Bitboard b2 = popLSB(attacks);
             int to = getLSBIndex(b2);
             bool cap = bool(oppOcc&(1ULL<<to));
             pseudo.push_back({from,to,NO_PIECE,cap,false,false});
         }
     }

     // King moves
     Bitboard king = pos.pieces[side==WHITE?WK:BK];
     int kfrom = getLSBIndex(king);
     Bitboard katt = kingAttacks[kfrom] & ~ownOcc;
     while (katt) {
         Bitboard b2 = popLSB(katt);
         int to = getLSBIndex(b2);
         bool cap = bool(oppOcc&(1ULL<<to));
         pseudo.push_back({kfrom,to,NO_PIECE,cap,false,false});
     }

     // Castling
     if (side == WHITE) {
         if (pos.castleRights[0] &&
             !(allOcc & ((1ULL<<5)|(1ULL<<6))) &&
             !isSquareAttacked(pos,4,BLACK) &&
             !isSquareAttacked(pos,5,BLACK) &&
             !isSquareAttacked(pos,6,BLACK)) {
             pseudo.push_back({4,6,NO_PIECE,false,false,true});
         }
         if (pos.castleRights[1] &&
             !(allOcc & ((1ULL<<1)|(1ULL<<2)|(1ULL<<3))) &&
             !isSquareAttacked(pos,4,BLACK) &&
             !isSquareAttacked(pos,3,BLACK) &&
             !isSquareAttacked(pos,2,BLACK)) {
             pseudo.push_back({4,2,NO_PIECE,false,false,true});
         }
     } else {
         if (pos.castleRights[2] &&
             !(allOcc & ((1ULL<<61)|(1ULL<<62))) &&
             !isSquareAttacked(pos,60,WHITE) &&
             !isSquareAttacked(pos,61,WHITE) &&
             !isSquareAttacked(pos,62,WHITE)) {
             pseudo.push_back({60,62,NO_PIECE,false,false,true});
         }
         if (pos.castleRights[3] &&
             !(allOcc & ((1ULL<<57)|(1ULL<<58)|(1ULL<<59))) &&
             !isSquareAttacked(pos,60,WHITE) &&
             !isSquareAttacked(pos,59,WHITE) &&
             !isSquareAttacked(pos,58,WHITE)) {
             pseudo.push_back({60,58,NO_PIECE,false,false,true});
         }
     }

     // Filter legal moves
     for (const Move& m : pseudo) {
         Position newPos = pos;
         makeMove(newPos, m);
         Color movedSide = side;
         int kingSq = getLSBIndex(newPos.pieces[movedSide==WHITE?WK:BK]);
         if (!isSquareAttacked(newPos, kingSq, newPos.sideToMove))
             moves.push_back(m);
     }
 }

 void makeMove(Position& pos, const Move& m) {
     Color side = pos.sideToMove;
     Color opp = side==WHITE?BLACK:WHITE;
     int piece = -1;
     for (int i = side*6; i < side*6+6; ++i) {
         if (pos.pieces[i] & (1ULL<<m.from)) {
             piece = i;
             break;
         }
     }
     pos.pieces[piece] &= ~(1ULL<<m.from);
     // Handle castling rook move
     if (m.castling) {
         if (piece == WK) {
             if (m.to == 6) {
                 pos.pieces[WR] &= ~(1ULL<<7);
                 pos.pieces[WR] |= (1ULL<<5);
             } else if (m.to == 2) {
                 pos.pieces[WR] &= ~(1ULL<<0);
                 pos.pieces[WR] |= (1ULL<<3);
             }
         } else if (piece == BK) {
             if (m.to == 62) {
                 pos.pieces[BR] &= ~(1ULL<<63);
                 pos.pieces[BR] |= (1ULL<<61);
             } else if (m.to == 58) {
                 pos.pieces[BR] &= ~(1ULL<<56);
                 pos.pieces[BR] |= (1ULL<<59);
             }
         }
     }
     // Handle captures
     if (m.capture) {
         if (m.enPassant) {
             int capSq = side==WHITE ? m.to-8 : m.to+8;
             int capPiece = side==WHITE ? BP : WP;
             pos.pieces[capPiece] &= ~(1ULL<<capSq);
         } else {
             for (int i = opp*6; i < opp*6+6; ++i) {
                 if (pos.pieces[i] & (1ULL<<m.to)) {
                     pos.pieces[i] &= ~(1ULL<<m.to);
                     break;
                 }
             }
         }
     }
     // Handle promotion or regular move
     if (m.promotion != NO_PIECE) {
         pos.pieces[m.promotion] |= (1ULL<<m.to);
     } else {
         pos.pieces[piece] |= (1ULL<<m.to);
     }
     // Update occupancies
     pos.occupancies[WHITE] = 0;
     pos.occupancies[BLACK] = 0;
     for (int i = 0; i < 6; ++i) pos.occupancies[WHITE] |= pos.pieces[i];
     for (int i = 6; i < 12; ++i) pos.occupancies[BLACK] |= pos.pieces[i];
     pos.occupancies[2] = pos.occupancies[WHITE] | pos.occupancies[BLACK];
     // Update castling rights
     if (piece == WK) { pos.castleRights[0] = pos.castleRights[1] = false; }
     else if (piece == BK) { pos.castleRights[2] = pos.castleRights[3] = false; }
     if (piece == WR) {
         if (m.from == 0) pos.castleRights[1] = false;
         else if (m.from == 7) pos.castleRights[0] = false;
     } else if (piece == BR) {
         if (m.from == 56) pos.castleRights[3] = false;
         else if (m.from == 63) pos.castleRights[2] = false;
     }
     if (m.capture && !m.enPassant) {
         if (m.to == 0) pos.castleRights[1] = false;
         else if (m.to == 7) pos.castleRights[0] = false;
         else if (m.to == 56) pos.castleRights[3] = false;
         else if (m.to == 63) pos.castleRights[2] = false;
     }
     // Update en passant square
     if ((piece == WP && m.to - m.from == 16) ||
         (piece == BP && m.from - m.to == 16)) {
         pos.enPassant = (m.from + m.to) / 2;
     } else {
         pos.enPassant = -1;
     }
     // Switch side
     pos.sideToMove = opp;
 }

 uint64_t perft(const Position& pos, int depth) {
     if (depth == 0) return 1;
     std::vector<Move> moves;
     generateLegalMoves(pos, moves);
     uint64_t nodes = 0;
     for (const Move& m : moves) {
         Position newPos = pos;
         makeMove(newPos, m);
         nodes += perft(newPos, depth-1);
     }
     return nodes;
 }

} // namespace chess