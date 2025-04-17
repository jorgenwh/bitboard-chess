 #include <iostream>
 #include <chrono>
 #include "board.h"

 int main(int argc, char* argv[]) {
     if (argc != 2) {
         std::cout << "Usage: " << argv[0] << " <depth>\n";
         return 1;
     }
     int depth = std::stoi(argv[1]);
     chess::Position pos;
     chess::initPosition(pos);
     auto start = std::chrono::high_resolution_clock::now();
     uint64_t nodes = chess::perft(pos, depth);
     auto end = std::chrono::high_resolution_clock::now();
     double secs = std::chrono::duration<double>(end - start).count();
     std::cout << "Perft(" << depth << ") : " << nodes << " nodes in " << secs << " seconds\n";
     return 0;
 }