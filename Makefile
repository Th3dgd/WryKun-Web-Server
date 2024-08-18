# Copyright (c) 2024 WryKun
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
INCLUDE = -Iinclude
SRC = $(wildcard src/*.cpp)
OBJ_DIR = objects
OBJ = $(patsubst src/%, $(OBJ_DIR)/%, $(SRC:.cpp=.o))
OUT = bin/wws

all: $(OUT)

$(OUT): $(OBJ)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(OBJ) $(INCLUDE) -o $(OUT)

$(OBJ_DIR)/%.o: src/%.cpp
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

run: $(OUT)
	cd bin && sudo ./wws

clean:
	rm -f $(OBJ) $(OUT)
	rm -rf $(OBJ_DIR)

.PHONY: all run clean
