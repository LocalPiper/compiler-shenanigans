CC = clang
AR = ar
CFLAGS = -g -Wall -Wextra -std=c11 -I../structs/include -Iinclude
LDFLAGS = -L../structs/libs -lstructs

LIB = libs/libthompson.a
BIN = bin/to_nfa

DOT_O = build/dot.o
MAIN_O = build/main.o
THOMPSON_O = build/thompson.o

all: $(LIB) $(BIN)

$(LIB): $(THOMPSON_O) | libs
	$(AR) rcs $@ $(THOMPSON_O)

$(BIN): $(DOT_O) $(MAIN_O) $(THOMPSON_O) | bin
	$(CC) $(DOT_O) $(MAIN_O) $(THOMPSON_O) $(LDFLAGS) -o $@

$(DOT_O): src/dot.c | build
	$(CC) $(CFLAGS) -c src/dot.c -o $@

$(MAIN_O): src/main.c | build
	$(CC) $(CFLAGS) -c src/main.c -o $@

$(THOMPSON_O): src/thompson.c | build
	$(CC) $(CFLAGS) -c src/thompson.c -o $@

libs bin build:
	mkdir -p $@

lib: $(LIB)

clean:
	rm -rf build/ bin/ libs/ output.dot output.png

.PHONY: all lib clean

