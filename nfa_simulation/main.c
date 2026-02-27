#include "../thompson_construction/thompson.h"
#include "../thompson_construction/stack.h"
#include "../subset_construction/generic_nfa.h"
#include "bitset.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

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

void eps_closure(GenericState* gs, StateSet *s) {
  Stack* st = stack_init();
  fill_stack(st, s);
  
  while (!stack_empty(st)) {
    GenericState* state = &gs[stack_pop(st)];

    for (Edge* e = state->edges; e != NULL; e = e->next) {
      if (e->label == -1 && !stateset_contains(s, e->target_id)) {
        stateset_add(s, e->target_id);
        stack_push(st, e->target_id);
      }
    }
  }

  stack_destroy(st);
}


StateSet* move(GenericState* gs, StateSet *s, int c) {
  StateSet* t = calloc(1, sizeof(StateSet));
  for (int word = 0; word < BITSET_WORDS; ++word) {
    uint64_t bits = s->bits[word];
    while (bits != 0) {
      int bitpos = __builtin_ctzll(bits);
      int state_id = word * 64 + bitpos;
      GenericState* state = &gs[state_id];
      for (Edge* e = state->edges; e != NULL; e = e->next) {
        if (e->label == c) stateset_add(t, e->target_id);
      }
      bits ^= (1LL << bitpos);
    }
  }

  return t;
}

bool simulate(GenericState* gs, int start_id, int end_id, char* str) {
  StateSet *S = calloc(1, sizeof(StateSet));
  stateset_add(S, start_id);

  eps_closure(gs, S);
  for (char* p = str; *p; ++p) {
    StateSet* old = S;
    S = move(gs, S, *p);
    eps_closure(gs, S);
    free(old);
  }

  bool res = stateset_contains(S, end_id);
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

  int num_states = 0, start_id = 0, end_id = 0;
  GenericState* nfa = thompson_construction(regex, &num_states, &start_id, &end_id);
  if (!nfa) {
    fprintf(stderr, "Provided empty regex\n");
    return 1;
  }
  for (int i = 0; i < n; ++i) {
    printf("%s %d\n", words[i], simulate(nfa, start_id, end_id, words[i]));
  }

  generic_graph_destroy(nfa, num_states);
}
