 #ifndef CHESS_MOVEGEN_H
 #define CHESS_MOVEGEN_H

 #include "position.h"
 #include "types.h"
 #include <vector>

namespace chess {

// Generate all legal moves for the given position
void generate_legal_moves(const Position &pos, std::vector<Move> &moves);

} // namespace chess

#endif // CHESS_MOVEGEN_H