 #include "perft.h"
 #include "movegen.h"
 #include <vector>

namespace chess {

uint64_t perft(const Position &pos, int depth) {
    if (depth == 0) return 1;
    std::vector<Move> moves;
    generate_legal_moves(pos, moves);
    uint64_t nodes = 0;
    for (const Move &m : moves) {
        Position new_pos = pos;
        make_move(new_pos, m);
        nodes += perft(new_pos, depth - 1);
    }
    return nodes;
}

} // namespace chess