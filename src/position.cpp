 #include "position.h"
 #include "bitboard.h"
#include <cstring>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>

namespace chess {

void init_position(Position &pos) {
    init_attack_tables();
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
    pos.side_to_move = WHITE;
    pos.en_passant = -1;
    pos.castle_rights = {true, true, true, true};
    pos.halfmove_clock = 0;
    pos.fullmove_clock = 1;
}

void make_move(Position &pos, const Move &m) {
    Color side = pos.side_to_move;
    Color opp = side == WHITE ? BLACK : WHITE;
    int piece_index = -1;
    for (int i = side*6; i < side*6+6; ++i) {
        if (pos.pieces[i] & (1ULL<<m.from)) {
            piece_index = i;
            break;
        }
    }
    // Remove moving piece from source square
    pos.pieces[piece_index] &= ~(1ULL<<m.from);

    // Handle castling rook move
    if (m.castling) {
        if (piece_index == WK) {
            if (m.to == 6) {
                pos.pieces[WR] &= ~(1ULL<<7);
                pos.pieces[WR] |= (1ULL<<5);
            } else if (m.to == 2) {
                pos.pieces[WR] &= ~(1ULL<<0);
                pos.pieces[WR] |= (1ULL<<3);
            }
        } else if (piece_index == BK) {
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
        if (m.en_passant) {
            int cap_sq = side == WHITE ? m.to-8 : m.to+8;
            int cap_piece = side == WHITE ? BP : WP;
            pos.pieces[cap_piece] &= ~(1ULL<<cap_sq);
        } else {
            for (int i = opp*6; i < opp*6+6; ++i) {
                if (pos.pieces[i] & (1ULL<<m.to)) {
                    pos.pieces[i] &= ~(1ULL<<m.to);
                    break;
                }
            }
        }
    }

    // Handle promotion or normal move
    if (m.promotion != NO_PIECE) {
        pos.pieces[m.promotion] |= (1ULL<<m.to);
    } else {
        pos.pieces[piece_index] |= (1ULL<<m.to);
    }

    // Update occupancies
    pos.occupancies[WHITE] = 0;
    pos.occupancies[BLACK] = 0;
    for (int i = 0; i < 6; ++i) pos.occupancies[WHITE] |= pos.pieces[i];
    for (int i = 6; i < 12; ++i) pos.occupancies[BLACK] |= pos.pieces[i];
    pos.occupancies[2] = pos.occupancies[WHITE] | pos.occupancies[BLACK];

    // Update castling rights
    if (piece_index == WK) { pos.castle_rights[0] = pos.castle_rights[1] = false; }
    else if (piece_index == BK) { pos.castle_rights[2] = pos.castle_rights[3] = false; }
    if (piece_index == WR) {
        if (m.from == 0) pos.castle_rights[1] = false;
        else if (m.from == 7) pos.castle_rights[0] = false;
    } else if (piece_index == BR) {
        if (m.from == 56) pos.castle_rights[3] = false;
        else if (m.from == 63) pos.castle_rights[2] = false;
    }
    if (m.capture && !m.en_passant) {
        if (m.to == 0) pos.castle_rights[1] = false;
        else if (m.to == 7) pos.castle_rights[0] = false;
        else if (m.to == 56) pos.castle_rights[3] = false;
        else if (m.to == 63) pos.castle_rights[2] = false;
    }

    // Update en passant square
    if ((piece_index == WP && m.to - m.from == 16) ||
        (piece_index == BP && m.from - m.to == 16)) {
        pos.en_passant = (m.from + m.to) / 2;
    } else {
        pos.en_passant = -1;
    }

    // Switch side to move
    pos.side_to_move = opp;
}

// Set position from FEN string; returns false on invalid FEN
bool set_fen(Position &pos, const std::string &fen) {
    std::istringstream iss(fen);
    std::string piece_placement, side_str, castling_str, ep_str;
    int halfmove, fullmove;
    if (!(iss >> piece_placement >> side_str >> castling_str >> ep_str >> halfmove >> fullmove))
        return false;
    // Clear piece bitboards
    std::memset(pos.pieces, 0, sizeof(pos.pieces));
    // Parse piece placement
    std::vector<std::string> ranks;
    std::istringstream riss(piece_placement);
    std::string rank_str;
    while (std::getline(riss, rank_str, '/')) ranks.push_back(rank_str);
    if (ranks.size() != 8) return false;
    for (int i = 0; i < 8; ++i) {
        const std::string &rstr = ranks[i];
        int file = 0;
        int rank = 7 - i;
        for (char c : rstr) {
            if (std::isdigit(c)) {
                file += c - '0';
            } else {
                if (file < 0 || file > 7) return false;
                int sq = rank * 8 + file;
                Piece p = NO_PIECE;
                switch (c) {
                    case 'P': p = WP; break;
                    case 'N': p = WN; break;
                    case 'B': p = WB; break;
                    case 'R': p = WR; break;
                    case 'Q': p = WQ; break;
                    case 'K': p = WK; break;
                    case 'p': p = BP; break;
                    case 'n': p = BN; break;
                    case 'b': p = BB; break;
                    case 'r': p = BR; break;
                    case 'q': p = BQ; break;
                    case 'k': p = BK; break;
                    default: return false;
                }
                pos.pieces[p] |= (1ULL << sq);
                file++;
            }
        }
        if (file != 8) return false;
    }
    // Update occupancies
    pos.occupancies[WHITE] = pos.occupancies[BLACK] = 0;
    for (int p = WP; p <= WK; ++p) pos.occupancies[WHITE] |= pos.pieces[p];
    for (int p = BP; p <= BK; ++p) pos.occupancies[BLACK] |= pos.pieces[p];
    pos.occupancies[2] = pos.occupancies[WHITE] | pos.occupancies[BLACK];
    // Side to move
    if (side_str == "w") pos.side_to_move = WHITE;
    else if (side_str == "b") pos.side_to_move = BLACK;
    else return false;
    // Castling rights
    pos.castle_rights = {false, false, false, false};
    if (castling_str.find('K') != std::string::npos) pos.castle_rights[0] = true;
    if (castling_str.find('Q') != std::string::npos) pos.castle_rights[1] = true;
    if (castling_str.find('k') != std::string::npos) pos.castle_rights[2] = true;
    if (castling_str.find('q') != std::string::npos) pos.castle_rights[3] = true;
    // En passant target
    if (ep_str == "-") {
        pos.en_passant = -1;
    } else {
        if (ep_str.size() != 2) return false;
        char f = ep_str[0]; char r = ep_str[1];
        if (f < 'a' || f > 'h' || r < '1' || r > '8') return false;
        int file = f - 'a';
        int rank = r - '1';
        pos.en_passant = rank * 8 + file;
    }
    // Halfmove and fullmove clocks
    pos.halfmove_clock = halfmove;
    pos.fullmove_clock = fullmove;
    return true;
}

// Get FEN string from position
std::string get_fen(const Position &pos) {
    std::ostringstream oss;
    for (int rank = 7; rank >= 0; --rank) {
        int empty = 0;
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            char pc = '.';
            for (int p = WP; p <= BK; ++p) {
                if (pos.pieces[p] & (1ULL << sq)) {
                    static const char map[] = {'P','N','B','R','Q','K','p','n','b','r','q','k'};
                    pc = map[p];
                    break;
                }
            }
            if (pc == '.') {
                empty++;
            } else {
                if (empty) { oss << empty; empty = 0; }
                oss << pc;
            }
        }
        if (empty) oss << empty;
        if (rank > 0) oss << '/';
    }
    // Side to move
    oss << ' ' << (pos.side_to_move == WHITE ? 'w' : 'b');
    // Castling rights
    oss << ' ';
    std::string cr;
    if (pos.castle_rights[0]) cr += 'K';
    if (pos.castle_rights[1]) cr += 'Q';
    if (pos.castle_rights[2]) cr += 'k';
    if (pos.castle_rights[3]) cr += 'q';
    oss << (cr.empty() ? "-" : cr);
    // En passant target
    oss << ' ';
    if (pos.en_passant < 0) {
        oss << '-';
    } else {
        int file = pos.en_passant % 8;
        int rank = pos.en_passant / 8;
        oss << char('a' + file) << char('1' + rank);
    }
    // Halfmove and fullmove clocks
    oss << ' ' << pos.halfmove_clock << ' ' << pos.fullmove_clock;
    return oss.str();
}

// Get ASCII diagram of the position
std::string position_to_string(const Position &pos) {
    std::ostringstream oss;
    for (int rank = 7; rank >= 0; --rank) {
        oss << rank + 1 << " ";
        for (int file = 0; file < 8; ++file) {
            int sq = rank * 8 + file;
            char pc = '.';
            for (int p = WP; p <= BK; ++p) {
                if (pos.pieces[p] & (1ULL << sq)) {
                    static const char map[] = {'P','N','B','R','Q','K','p','n','b','r','q','k'};
                    pc = map[p];
                    break;
                }
            }
            oss << pc;
            if (file < 7) oss << ' ';
        }
        oss << '\n';
    }
    oss << "  a b c d e f g h\n";
    return oss.str();
}

} // namespace chess