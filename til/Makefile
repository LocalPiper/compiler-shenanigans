CXX := clang++
CXXFLAGS := -std=c++20 -g -Wall -Wextra -Iinclude

SRC := $(wildcard src/*.cpp)
HDR := $(wildcard include/*.hpp)
OBJ := $(SRC:src/%.cpp=build/%.o)

ALL := $(SRC) $(HDR)

TARGET := til

$(TARGET): $(OBJ)
	$(CXX) $(OBJ) -o $@

build/%.o: src/%.cpp
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -c $< -o $@

format:
	clang-format --style=LLVM -i $(ALL)

clean:
	rm -rf build $(TARGET)

.PHONY: clean
