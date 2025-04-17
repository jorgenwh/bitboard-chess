 #ifndef CHESS_BOARD_H
 #define CHESS_BOARD_H

 #include <cstdint>
 #include <vector>

 namespace chess {

 using Bitboard = uint64_t;

 enum Piece {
     WP, WN, WB, WR, WQ, WK,
     BP, BN, BB, BR, BQ, BK,
     NO_PIECE
 };

 enum Color { WHITE = 0, BLACK = 1 };

 struct Move {
     int from;
     int to;
     Piece promotion;
     bool capture;
     bool enPassant;
     bool castling;
 };

 struct Position {
     Bitboard pieces[12];
     Bitboard occupancies[3];
     Color sideToMove;
     int enPassant;
     bool castleRights[4];
 };

 void initPosition(Position& pos);
 void generateLegalMoves(const Position& pos, std::vector<Move>& moves);
 void makeMove(Position& pos, const Move& move);
 uint64_t perft(const Position& pos, int depth);

 } // namespace chess

 #endif // CHESS_BOARD_H