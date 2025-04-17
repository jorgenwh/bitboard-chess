cmake_minimum_required(VERSION 3.10)
project(ChessPerft)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
include_directories(src)
add_executable(chessperft
    src/main.cpp
    src/bitboard.cpp
    src/position.cpp
    src/movegen.cpp
    src/perft.cpp
)