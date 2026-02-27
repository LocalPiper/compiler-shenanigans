CC = clang
AR = ar
CFLAGS = -g -Wall -Wextra -std=c11 -Iinclude -I.

SRCS = $(wildcard src/*.c)
OBJS = $(patsubst src/%.c,build/%.o,$(SRCS))
LIB = libs/libstructs.a

all: $(LIB)

$(LIB): $(OBJS)
	@mkdir -p libs
	$(AR) rcs $@ $(OBJS)

build/%.o: src/%.c
	@mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/ libs/

.PHONY: all clean

