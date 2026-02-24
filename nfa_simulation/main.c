#include "../thompson_construction/nfa.h"
#include "../thompson_construction/thompson.h"
#include "../thompson_construction/stack.h"
#include "bitset.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

State* id_to_state[1024];

void fill_list(State *nfa) {
  if (!nfa || id_to_state[nfa->id]) return;
  id_to_state[nfa->id] = nfa;
  fill_list(nfa->out1);
  fill_list(nfa->out2);
}

void fill_stack(Stack* st, StateSet *s) {
  for (int word = 0; word < BITSET_WORDS; ++word) {
    uint64_t bits = s->bits[word];
    while (bits != 0) {
      int bitpos = __builtin_ctzll(bits);
      int state_id = word * 64 + bitpos;
      stack_push(st, state_id);
      bits ^= (1LL << bitpos);
    }
  }
}

void eps_closure(StateSet *s) {
  Stack* st = stack_init();
  fill_stack(st, s);
  
  while (!stack_empty(st)) {
    State* t = id_to_state[stack_pop(st)];
    if (t->c == -1) {
      State* u = t->out1;
      if (u && !stateset_contains(s, u->id)) {
        stateset_add(s, u->id);
        stack_push(st, u->id);
      }
      u = t->out2;
      if (u && !stateset_contains(s, u->id)) {
        stateset_add(s, u->id);
        stack_push(st, u->id);
      }
    }
  }
}

StateSet* move(StateSet *s, int c) {
  StateSet* t = calloc(1, sizeof(StateSet));
  for (int word = 0; word < BITSET_WORDS; ++word) {
    uint64_t bits = s->bits[word];
    while (bits != 0) {
      int bitpos = __builtin_ctzll(bits);
      int state_id = word * 64 + bitpos;
      State* u = id_to_state[state_id];
      if (u->c == c) stateset_add(t, u->out1->id);
      bits ^= (1LL << bitpos);
    }
  }

  return t;
}

bool simulate(State* start, State* end, char* str) {
  StateSet *S = calloc(1, sizeof(StateSet));
  stateset_add(S, start->id);

  eps_closure(S);
  for (char* p = str; *p; ++p) {
    S = move(S, *p);
    eps_closure(S);
  }

  bool res = stateset_contains(S, end->id);
  free(S);
  return res;
}

int main(int argc, char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <regex> <string> [<string>...]\n", argv[0]);
    return 1;
  }
  char* regex = argv[1];
  char** words = argv + 2;
  int n = argc - 2;

  State** nfa = thompson_construction(regex);
  State *start = nfa[0], *end = nfa[1];

  fill_list(start);

  for (int i = 0; i < n; ++i) {
    printf("%s %d\n", words[i], simulate(start, end, words[i]));
  }

}
