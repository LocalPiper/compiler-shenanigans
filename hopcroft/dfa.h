#ifndef DFA_H
#define DFA_H

#define MAX_STATES 100
#define ALPHABET_SIZE 10

#include <stdbool.h>
#include <stdlib.h>

typedef struct State {
  int id;
  bool is_accepting;
  int next[ALPHABET_SIZE];
} State;

typedef struct DFA {
  State states[MAX_STATES];
  size_t sz;
  int initial;
} DFA;

#endif // !DFA_H
