#include "generic_nfa.h"
#include "../nfa_simulation/bitset.h"
#include "../thompson_construction/stack.h"
#include "../thompson_construction/nfa.h"
#include "../thompson_construction/thompson.h"
#include "dot.h"
#include "dfa_table.h"
#include <stdio.h>
#include <stdlib.h>


#define ALPHABET_SIZE 256
#define MAX_STATES 1024

State* id_to_state[MAX_STATES];

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

StateSet* compute_final_states(GenericState* gs, int num_states) {
  StateSet* final = calloc(1, sizeof(StateSet));
  for (int i = 0; i < num_states; ++i) {
    if (gs[i].is_final) stateset_add(final, i);
  }
  return final;
}

// note: list must be filled
GenericState* thompson_to_generic(State* nfa, int* num_states_out) {
  if (!nfa) return NULL;

  int num_states = 0;
  for (int i = 0; i < MAX_STATES; i++) {
    if (id_to_state[i]) num_states = i + 1;
  }

  GenericState* states = calloc(num_states, sizeof(GenericState));
  for (int i = 0; i < num_states; ++i) {
    states[i].id = i;
    states[i].is_final = id_to_state[i]->is_final;
    states[i].edges = NULL;
  }

  for (int i = 0; i < num_states; ++i) {
    State* src = id_to_state[i];
    
    if (src->out1) {
      Edge* e = malloc(sizeof(Edge));
      e->label = src->c;
      e->target_id = src->out1->id;
      e->next = states[i].edges;
      states[i].edges = e;
    }

    if (src->out2) {
      Edge* e = malloc(sizeof(Edge));
      e->label = src->c;
      e->target_id = src->out2->id;
      e->next = states[i].edges;
      states[i].edges = e;
    }
  }

  *num_states_out = num_states;
  return states;
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

DFATable* encode_new_states(GenericState* nfa, int num_states) {
  DFATable* table = dfa_table_init();
  StateSet* F = compute_final_states(nfa, num_states);

  StateSet* S = calloc(1, sizeof(StateSet));
  stateset_add(S, 0);
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
        } else {
          int new_id = dfa_table_add_state(table, T);
          table->table[new_id]->is_final = stateset_check_intersection(T, F);
          entry->char_to_id[c] = new_id;
          unprocessed_states++;
        }
      } else {
        entry->char_to_id[c] = -1;
      }

    }
    ++curr_state;
  }

  generic_graph_destroy(nfa, num_states);
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

  State** nfa = thompson_construction(regex);
  fill_list(nfa[0]);

  int num_states = 0;
  DFATable* dfa = encode_new_states(thompson_to_generic(nfa[0], &num_states), num_states);
  nfa_destroy(nfa[0]);
  free(nfa);

  write_dot_file(dfa);

  for (int i = 0; i < n; ++i) {
    printf("%s %d\n", words[i], simulate(dfa, words[i]));
  }

  dfa_table_destroy(dfa);
}

