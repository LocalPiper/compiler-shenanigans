#include "dfa.h"
#include "bitset.h"
#include "automaton.h"

#define ALPHABET_SIZE 256

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

AutomatonNode* dfa_table_to_dfa(DFATable* table, int* num_states_out) {
  *num_states_out = table->num_states;
  AutomatonNode* dfa = calloc(table->num_states, sizeof(AutomatonNode));
  for (int i = 0; i < table->num_states; ++i) {
    DFATableEntry* entry = table->table[i];
    dfa[i].id = i;
    dfa[i].is_final = entry->is_final;
    dfa[i].edges = NULL;
    for (int c = 0; c < ALPHABET_SIZE; ++c) {
      if (entry->char_to_id[c] != -1) {
        AutomatonEdge* e = calloc(1, sizeof(AutomatonEdge));
        e->label = c;
        e->target_id = entry->char_to_id[c];
        e->next = dfa[i].edges;
        dfa[i].edges = e;
      }
    }
  }
  return dfa;
}
