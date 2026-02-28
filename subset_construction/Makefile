CC = clang
CFLAGS = -g -Wall -Wextra -std=c11 -Iinclude -I../structs/include -I../thompson_construction/include
LDFLAGS = -L../structs/libs -lstructs -L../thompson_construction/libs -lthompson
TARGET = bin/to_dfa
SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,build/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) | bin
	$(CC) $(OBJS) $(LDFLAGS) -o $@

build/%.o: src/%.c | build
	$(CC) $(CFLAGS) -c $< -o $@

bin build:
	mkdir -p $@

clean:
	rm -rf build/ bin/ output.dot output.png

.PHONY: all clean

