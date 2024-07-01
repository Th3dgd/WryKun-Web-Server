CXX = g++
SRC = src/*.cpp
INCLUDE = -Iinclude
OUT = bin/wws

all: $(OUT)

$(OUT): $(SRC)
	$(CXX) $(SRC) $(INCLUDE) -o $(OUT)

run: $(OUT)
	cd bin && sudo ./wws

clean:
	rm -f $(OUT)

.PHONY: all run clean
