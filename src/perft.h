 #ifndef CHESS_PERFT_H
 #define CHESS_PERFT_H

 #include "position.h"
 #include <cstdint>

namespace chess {

// Perft calculates the number of leaf nodes at a given search depth
uint64_t perft(const Position &pos, int depth);

} // namespace chess

#endif // CHESS_PERFT_H