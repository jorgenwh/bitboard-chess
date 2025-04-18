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

# Enable CTest integration
enable_testing()

# Perft correctness tests
add_test(NAME perft_1 COMMAND $<TARGET_FILE:chessperft> 1)
set_tests_properties(perft_1 PROPERTIES PASS_REGULAR_EXPRESSION "Perft\\(1\\) : 20 nodes")
add_test(NAME perft_2 COMMAND $<TARGET_FILE:chessperft> 2)
set_tests_properties(perft_2 PROPERTIES PASS_REGULAR_EXPRESSION "Perft\\(2\\) : 400 nodes")
add_test(NAME perft_3 COMMAND $<TARGET_FILE:chessperft> 3)
set_tests_properties(perft_3 PROPERTIES PASS_REGULAR_EXPRESSION "Perft\\(3\\) : 8902 nodes")
add_test(NAME perft_4 COMMAND $<TARGET_FILE:chessperft> 4)
set_tests_properties(perft_4 PROPERTIES PASS_REGULAR_EXPRESSION "Perft\\(4\\) : 197281 nodes")
add_test(NAME perft_5 COMMAND $<TARGET_FILE:chessperft> 5)
set_tests_properties(perft_5 PROPERTIES PASS_REGULAR_EXPRESSION "Perft\\(5\\) : 4865609 nodes")
add_test(NAME perft_6 COMMAND $<TARGET_FILE:chessperft> 6)
set_tests_properties(perft_6 PROPERTIES PASS_REGULAR_EXPRESSION "Perft\\(6\\) : 119060324 nodes")