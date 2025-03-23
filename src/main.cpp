#include "bitboard.h"
#include "board.h"
#include "movegen.h"
#include "perft.h"

#include <iostream>
#include <string>
#include <cstring>

#define START_FEN "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

int main(int argc, char* argv[])
{
    Board board;

    std::cout << Bitboards::to_string(board.get_occupied()) << std::endl;
    std::cout << Bitboards::to_string(board.get_empty()) << std::endl;

    return 0;
}
