#include "board.h"
#include "types.h"

Board::Board()
{
    reset();
}

Board::Board(const std::string& fen)
{
    from_fen(fen);
}

Bitboard Board::get_occupied() const
{
    return (
        pieces[WHITE][PAWN-1]   | pieces[WHITE][KNIGHT-1] |
        pieces[WHITE][BISHOP-1] | pieces[WHITE][ROOK-1]   |
        pieces[WHITE][QUEEN-1]  | pieces[WHITE][KING-1]   |
        pieces[BLACK][PAWN-1]   | pieces[BLACK][KNIGHT-1] |
        pieces[BLACK][BISHOP-1] | pieces[BLACK][ROOK-1]   |
        pieces[BLACK][QUEEN-1]  | pieces[BLACK][KING-1]
    );
}

Bitboard Board::get_occupied(Color color) const
{
    return (
        pieces[color][PAWN-1]   | pieces[color][KNIGHT-1] |
        pieces[color][BISHOP-1] | pieces[color][ROOK-1]   |
        pieces[color][QUEEN-1]  | pieces[color][KING-1]
    );
}

Bitboard Board::get_empty() const
{
    return ~get_occupied();
}

void Board::reset()
{
    // clear all bitboards
    for (int c = 0; c < Color::COLOR_NB; c++)
    {
        for (int p = 0; p < PieceType::PIECE_NB; p++)
        {
            pieces[c][p] = 0ULL;
        }
    }

    // white pieces
    pieces[Color::WHITE][PieceType::PAWN - 1] = Bitboards::RANK_2;
    pieces[Color::WHITE][PieceType::KNIGHT - 1] = Bitboards::square_bb(B1) | Bitboards::square_bb(G1);
    pieces[Color::WHITE][PieceType::BISHOP - 1] = Bitboards::square_bb(C1) | Bitboards::square_bb(F1);
    pieces[Color::WHITE][PieceType::ROOK - 1] = Bitboards::square_bb(A1) | Bitboards::square_bb(H1);
    pieces[Color::WHITE][PieceType::QUEEN - 1] = Bitboards::square_bb(D1);
    pieces[Color::WHITE][PieceType::KING - 1] = Bitboards::square_bb(E1);

    // black pieces
    pieces[Color::BLACK][PieceType::PAWN - 1] = Bitboards::RANK_7;
    pieces[Color::BLACK][PieceType::KNIGHT - 1] = Bitboards::square_bb(B8) | Bitboards::square_bb(G8);
    pieces[Color::BLACK][PieceType::BISHOP - 1] = Bitboards::square_bb(C8) | Bitboards::square_bb(F8);
    pieces[Color::BLACK][PieceType::ROOK - 1] = Bitboards::square_bb(A8) | Bitboards::square_bb(H8);
    pieces[Color::BLACK][PieceType::QUEEN - 1] = Bitboards::square_bb(D8);
    pieces[Color::BLACK][PieceType::KING - 1] = Bitboards::square_bb(E8);

    side_to_move = Color::WHITE;
    en_passant_square = SQUARE_NB;
    castling_rights = WHITE_KINGSIDE | WHITE_QUEENSIDE | BLACK_KINGSIDE | BLACK_QUEENSIDE;
}

bool Board::from_fen(const std::string& fen)
{
    // clear board
    for (int c = 0; c < Color::COLOR_NB; c++)
    {
        for (int p = 0; p < PieceType::PIECE_NB; p++)
        {
            pieces[c][p] = 0ULL;
        }
    }

    std::istringstream ss(fen);
    std::string token;

    ss >> token;

    // parse board position
    int rank = 7; // start at 8th rank (0-indexed as 7)
    int file = 0;

    for (char c : token)
    {
        if (c == '/')
        {
            rank--;
            file = 0;
        }
        else if (std::isdigit(c))
        {
            file += c - '0';
        }
        else 
        {
            Color color = std::isupper(c) ? Color::WHITE : Color::BLACK;
            PieceType piece;

            switch (std::toupper(c))
            {
                case 'P': piece = PieceType::PAWN; break;
                case 'N': piece = PieceType::KNIGHT; break;
                case 'B': piece = PieceType::BISHOP; break;
                case 'R': piece = PieceType::ROOK; break;
                case 'Q': piece = PieceType::QUEEN; break;
                case 'K': piece = PieceType::KING; break;
                default: return false; // invalid piece
            }

            Square sq = static_cast<Square>(rank*8 + file);
            pieces[color][piece - 1] |= Bitboards::square_bb(sq);
            file++;
        }
    }

    // side to move
    if (!(ss >> token)) return false;
    side_to_move = (token == "w") ? Color::WHITE : Color::BLACK;

    // castling rights
    if (!(ss >> token)) return false;
    castling_rights = 0;
    for (char c : token)
    {
        if (c == 'K') castling_rights |= WHITE_KINGSIDE;
        else if (c == 'Q') castling_rights |= WHITE_QUEENSIDE;
        else if (c == 'k') castling_rights |= BLACK_KINGSIDE;
        else if (c == 'q') castling_rights |= BLACK_QUEENSIDE;
    }

    // en passant square
    if (!(ss >> token)) return false;
    if (token == "-")
    {
        en_passant_square = SQUARE_NB;
    }
    else
    {
        file = token[0] - 'a';
        rank = token[1] - '1';
        en_passant_square = static_cast<Square>(rank*8 + file);
    }

    return true;
}

std::string Board::get_fen() const
{
    std::stringstream ss;

    // board position
    for (int rank = 7; rank >= 0; rank--)
    {
        int empty_count = 0;

        for (int file = 0; file < 8; file++)
        {
            Square sq = static_cast<Square>(rank * 8 + file);
            Bitboard sq_bb = Bitboards::square_bb(sq);

            char piece = 0;

            if (sq_bb & pieces[WHITE][PAWN-1]) piece = 'P';
            else if (sq_bb & pieces[WHITE][KNIGHT-1]) piece = 'N';
            else if (sq_bb & pieces[WHITE][BISHOP-1]) piece = 'B';
            else if (sq_bb & pieces[WHITE][ROOK-1]) piece = 'R';
            else if (sq_bb & pieces[WHITE][QUEEN-1]) piece = 'Q';
            else if (sq_bb & pieces[WHITE][KING-1]) piece = 'K';

            else if (sq_bb & pieces[BLACK][PAWN-1]) piece = 'p';
            else if (sq_bb & pieces[BLACK][KNIGHT-1]) piece = 'n';
            else if (sq_bb & pieces[BLACK][BISHOP-1]) piece = 'b';
            else if (sq_bb & pieces[BLACK][ROOK-1]) piece = 'r';
            else if (sq_bb & pieces[BLACK][QUEEN-1]) piece = 'q';
            else if (sq_bb & pieces[BLACK][KING-1]) piece = 'k';

            if (piece)
            {
                if (empty_count > 0)
                {
                    ss << empty_count;
                    empty_count = 0;
                }
                ss << piece;
            } 
            else 
            {
                empty_count++;
            }
        }

        if (empty_count > 0)
        {
            ss << empty_count;
        }

        if (rank > 0)
        {
            ss << '/';
        }
    }

    // side to move
    ss << ' ' << (side_to_move == WHITE ? 'w' : 'b');

    // castling rights
    ss << ' ';
    if (castling_rights == 0)
    {
        ss << '-';
    }
    else
    {
        if (castling_rights & WHITE_KINGSIDE) ss << 'K';
        if (castling_rights & WHITE_QUEENSIDE) ss << 'Q';
        if (castling_rights & BLACK_KINGSIDE) ss << 'k';
        if (castling_rights & BLACK_QUEENSIDE) ss << 'q';
    }

    // en passant square
    ss << ' ';
    if (en_passant_square == SQUARE_NB)
    {
        ss << '-';
    }
    else
    {
        char file = 'a' + (en_passant_square % 8);
        char rank = '1' + (en_passant_square / 8);
        ss << file << rank;
    }

    // halfmove clock and fullmove number
    // for now just use 0 for halfmove and 1 for fullmove as placeholders
    ss << " 0 1";

    return ss.str();
}

std::string Board::to_string() const
{
    std::string result = "  +---+---+---+---+---+---+---+---+\n";

    for (int rank = 7; rank >= 0; rank--)
    {
        result += std::to_string(rank + 1) + " |";

        for (int file = 0; file < 8; file++)
        {
            Square sq = static_cast<Square>(rank * 8 + file);
            Bitboard sqBB = Bitboards::square_bb(sq);

            char piece = ' ';

            if (sqBB & pieces[WHITE][PAWN-1]) piece = 'P';
            else if (sqBB & pieces[WHITE][KNIGHT-1]) piece = 'N';
            else if (sqBB & pieces[WHITE][BISHOP-1]) piece = 'B';
            else if (sqBB & pieces[WHITE][ROOK-1]) piece = 'R';
            else if (sqBB & pieces[WHITE][QUEEN-1]) piece = 'Q';
            else if (sqBB & pieces[WHITE][KING-1]) piece = 'K';

            else if (sqBB & pieces[BLACK][PAWN-1]) piece = 'p';
            else if (sqBB & pieces[BLACK][KNIGHT-1]) piece = 'n';
            else if (sqBB & pieces[BLACK][BISHOP-1]) piece = 'b';
            else if (sqBB & pieces[BLACK][ROOK-1]) piece = 'r';
            else if (sqBB & pieces[BLACK][QUEEN-1]) piece = 'q';
            else if (sqBB & pieces[BLACK][KING-1]) piece = 'k';

            result += " " + std::string(1, piece) + " |";
        }
        result += "\n  +---+---+---+---+---+---+---+---+\n";
    }
    result += "    a   b   c   d   e   f   g   h\n\n";

    result += "Side to move: " + std::string(side_to_move == WHITE ? "White" : "Black") + "\n";
    result += "Castling rights: ";

    if (castling_rights == 0)
    {
        result += "none";
    }
    else
    {
        if (castling_rights & WHITE_KINGSIDE) result += "K";
        if (castling_rights & WHITE_QUEENSIDE) result += "Q";
        if (castling_rights & BLACK_KINGSIDE) result += "k";
        if (castling_rights & BLACK_QUEENSIDE) result += "q";
    }

    if (en_passant_square != SQUARE_NB)
    {
        char file = 'a' + (en_passant_square % 8);
        char rank = '1' + (en_passant_square / 8);
        result += "\nEn passant square: " + std::string(1, file) + std::string(1, rank);
    }

    return result;
}
