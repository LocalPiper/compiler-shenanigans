#include "generic_nfa.h"
#include "../nfa_simulation/bitset.h"
#include "../thompson_construction/stack.h"
#include "../thompson_construction/thompson.h"
#include "dot.h"
#include "dfa_table.h"
#include <stdio.h>
#include <stdlib.h>


#define ALPHABET_SIZE 256
#define MAX_STATES 1024

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

StateSet* compute_final_states(GenericState* gs, int num_states) {
  StateSet* final = calloc(1, sizeof(StateSet));
  for (int i = 0; i < num_states; ++i) {
    if (gs[i].is_final) stateset_add(final, i);
  }
  return final;
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

DFATable* encode_new_states(GenericState* nfa, int start_id, int num_states) {
  DFATable* table = dfa_table_init();
  StateSet* F = compute_final_states(nfa, num_states);

  StateSet* S = calloc(1, sizeof(StateSet));
  stateset_add(S, start_id);
  eps_closure(nfa, S);
  dfa_table_add_state(table, S);
  table->table[0]->is_final = stateset_check_intersection(S, F);
  
  int curr_state = 0;
  int unprocessed_states = 1;

  while (curr_state < unprocessed_states) {
    DFATableEntry* entry = table->table[curr_state];

    for (int c = 0; c < ALPHABET_SIZE; ++c) {
      StateSet* T = move(nfa, entry->nfa_states, c);
      eps_closure(nfa, T);

      if (!stateset_empty(T)) {
        int id = dfa_table_find_state(table, T);
        if (id != -1) {
          entry->char_to_id[c] = id;
          free(T);
        } else {
          int new_id = dfa_table_add_state(table, T);
          table->table[new_id]->is_final = stateset_check_intersection(T, F);
          entry->char_to_id[c] = new_id;
          unprocessed_states++;
        }
      } else {
        entry->char_to_id[c] = -1;
        free(T);
      }

    }
    ++curr_state;
  }

  free(F);
  return table;
}

bool simulate(DFATable* dfa, char* word) {
  DFATableEntry* state = dfa->table[0];
  for (char* p = word; *p; ++p) {
    if (state->char_to_id[(int)(*p)] == -1) return false;
    state = dfa->table[state->char_to_id[(int)(*p)]];
  }
  return state->is_final;
}

int main(int argc, char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <regex> <string> [<string>...]\n", argv[0]);
    return 1;
  }
  char* regex = argv[1];
  char** words = argv + 2;
  int n = argc - 2;

  int num_states = 0;
  int start_state_id = -1;
  GenericState* nfa = thompson_construction(regex, &num_states, &start_state_id, NULL);
  if (!nfa) {
    fprintf(stderr, "Provided empty regex\n");
    return 1;
  }
  DFATable* dfa = encode_new_states(nfa, start_state_id, num_states);
  generic_graph_destroy(nfa, num_states);

  write_dot_file(dfa);

  for (int i = 0; i < n; ++i) {
    printf("%s %d\n", words[i], simulate(dfa, words[i]));
  }

  dfa_table_destroy(dfa);
}

