CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra
SRC = $(wildcard src/*.cpp)
INCLUDE = -Iinclude
OUT = bin/wws

all: $(OUT)

$(OUT): $(SRC)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(SRC) $(INCLUDE) -o $(OUT)

run: $(OUT)
	cd bin && sudo ./wws

clean:
	rm -f $(OUT)

.PHONY: all run clean