CC      = clang
CFLAGS  = -g -Wall -Wextra -std=c11

TARGET  = a.out

SRC = \
  ../thompson_construction/thompson.c \
  ../thompson_construction/stack.c \
  ../thompson_construction/nfa.c \
  main.c

OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

