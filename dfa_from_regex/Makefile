CC = clang
CFLAGS = -Wall -Wextra -std=c11 -Iinclude -I../structs/include -I../thompson_construction/include -I../subset_construction/include
DEBUGFLAGS = -g -O0 -DDEBUG
LDFLAGS = -L../structs/libs -lstructs -L../thompson_construction/libs -lthompson -L../subset_construction/libs -ldfa

BIN = bin/to_dfa
BIN_OBJS = build/dot.o build/main.o build/syntax_tree.o build/direct.o

all: $(LIB) $(BIN)

$(BIN): $(BIN_OBJS) | bin
	$(CC) $(BIN_OBJS) $(LDFLAGS) $(DEBUGFLAGS) -o $@

build/syntax_tree.o: src/syntax_tree.c | build
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c src/syntax_tree.c -o $@

build/direct.o: src/direct.c | build
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c src/direct.c -o $@

build/dot.o: src/dot.c | build
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c src/dot.c -o $@

build/main.o: src/main.c | build
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c src/main.c -o $@

libs bin build:
	mkdir -p $@

lib: $(LIB)

clean:
	rm -rf build/ bin/ libs/ output.dot output.png

.PHONY: all lib clean

