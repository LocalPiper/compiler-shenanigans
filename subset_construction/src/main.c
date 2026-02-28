#include "automaton.h"
#include "bitset.h"
#include "thompson.h"
#include "dot.h"
#include "dfa_table.h"
#include <stdio.h>
#include <stdlib.h>


#define ALPHABET_SIZE 256
#define MAX_STATES 1024

StateSet* compute_final_states(AutomatonNode* gs, int num_states) {
  StateSet* final = calloc(1, sizeof(StateSet));
  for (int i = 0; i < num_states; ++i) {
    if (gs[i].is_final) stateset_add(final, i);
  }
  return final;
}

DFATable* encode_new_states(AutomatonNode* nfa, int start_id, int num_states) {
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
        } else { // go go gadget ownership transfer
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
  AutomatonNode* nfa = thompson_construction(regex, &num_states, &start_state_id, NULL);
  if (!nfa) {
    fprintf(stderr, "Provided empty regex\n");
    return 1;
  }
  DFATable* dfa = encode_new_states(nfa, start_state_id, num_states);
  automaton_destroy(nfa, num_states);

  write_dot_file(dfa);

  for (int i = 0; i < n; ++i) {
    printf("%s %d\n", words[i], simulate(dfa, words[i]));
  }

  dfa_table_destroy(dfa);
}

