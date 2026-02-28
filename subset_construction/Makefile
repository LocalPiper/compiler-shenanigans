CC = clang
AR = ar
CFLAGS = -g -Wall -Wextra -std=c11 -Iinclude -I../structs/include -I../thompson_construction/include
LDFLAGS = -L../structs/libs -lstructs -L../thompson_construction/libs -lthompson

LIB = libs/libdfa.a
LIB_OBJS = build/dfa.o build/dfa_table.o

BIN = bin/to_dfa
BIN_OBJS = build/dot.o build/main.o build/dfa.o build/dfa_table.o

all: $(LIB) $(BIN)

$(LIB): $(LIB_OBJS) | libs
	$(AR) rcs $@ $(LIB_OBJS)

$(BIN): $(BIN_OBJS) | bin
	$(CC) $(BIN_OBJS) $(LDFLAGS) -o $@

build/dfa.o: src/dfa.c | build
	$(CC) $(CFLAGS) -c src/dfa.c -o $@

build/dfa_table.o: src/dfa_table.c | build
	$(CC) $(CFLAGS) -c src/dfa_table.c -o $@

build/dot.o: src/dot.c | build
	$(CC) $(CFLAGS) -c src/dot.c -o $@

build/main.o: src/main.c | build
	$(CC) $(CFLAGS) -c src/main.c -o $@

libs bin build:
	mkdir -p $@

lib: $(LIB)

clean:
	rm -rf build/ bin/ libs/ output.dot output.png

.PHONY: all lib clean

