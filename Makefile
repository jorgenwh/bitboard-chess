CXX = g++
CXXFLAGS = -std=c++20 -O3 -Wall -Wextra

SRC_DIR = src

OBJ = $(SRC_DIR)/bitboard.o $(SRC_DIR)/board.o $(SRC_DIR)/perft.o $(SRC_DIR)/main.o
TARGET = ce

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(SRC_DIR)/*.o $(TARGET)
