#include "automaton.h"
#include "bitset.h"
#include "stack.h"
#include <stdlib.h>

void fill_stack(Stack* St, StateSet *S) {
  for (int word = 0; word < BITSET_WORDS; ++word) {
    uint64_t bits = S->bits[word];
    while (bits != 0) {
      int bitpos = __builtin_ctzll(bits);
      int state_id = word * 64 + bitpos;
      stack_push(St, state_id);
      bits ^= (1LL << bitpos);
    }
  }
}

void eps_closure(AutomatonNode* G, StateSet* S) {
  Stack* St = stack_init();
  fill_stack(St, S);
  
  while (!stack_empty(St)) {
    AutomatonNode* U = &G[stack_pop(St)];

    for (AutomatonEdge* e = U->edges; e != NULL; e = e->next) {
      if (e->label == -1 && !stateset_contains(S, e->target_id)) {
        stateset_add(S, e->target_id);
        stack_push(St, e->target_id);
      }
    }
  }

  stack_destroy(St);
}

StateSet* move(AutomatonNode* G, StateSet* S, int c) {
  StateSet* T = calloc(1, sizeof(StateSet));
  for (int word = 0; word < BITSET_WORDS; ++word) {
    uint64_t bits = S->bits[word];
    while (bits != 0) {
      int bitpos = __builtin_ctzll(bits);
      int state_id = word * 64 + bitpos;
      AutomatonNode* U = &G[state_id];
      for (AutomatonEdge* e = U->edges; e != NULL; e = e->next) {
        if (e->label == c) stateset_add(T, e->target_id);
      }
      bits ^= (1LL << bitpos);
    }
  }

  return T;
}

void automaton_destroy(AutomatonNode* G, int n) {
  if (!G) return;
  for (int i = 0; i < n; ++i) {
    AutomatonEdge* e = G[i].edges;
    while (e) {
      AutomatonEdge* next = e->next;
      free(e);
      e = next;
    }
  }
  free(G);
}
