 #ifndef CHESS_TYPES_H
 #define CHESS_TYPES_H

 #include <cstdint>

namespace chess {

using Bitboard = uint64_t;

enum Piece {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK,
    NO_PIECE
};

enum Color {
    WHITE = 0,
    BLACK = 1
};

struct Move {
    int from;
    int to;
    Piece promotion;
    bool capture;
    bool en_passant;
    bool castling;
};

} // namespace chess

#endif // CHESS_TYPES_H