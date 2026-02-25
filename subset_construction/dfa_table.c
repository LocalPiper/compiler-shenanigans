#include "dfa_table.h"

DFATable* dfa_table_init() {
  DFATable* table = malloc(sizeof(DFATable));
  table->capacity = 16;
  table->num_states = 0;
  table->table = calloc(16, sizeof(DFATableEntry*));
  return table;
}

int dfa_table_find_state(DFATable* dfatable, StateSet* nfa_states) {
  for (int i = 0; i < dfatable->num_states; ++i) {
    if (stateset_equal(nfa_states, dfatable->table[i]->nfa_states)) return i;
  }
  return -1;
}

void dfa_table_grow(DFATable* dfatable) {
  dfatable->capacity <<= 1;
  dfatable->table = realloc(dfatable->table, dfatable->capacity * sizeof(DFATableEntry*));
}

int dfa_table_add_state(DFATable* dfatable, StateSet* nfa_states) {
  if (dfatable->num_states >= dfatable->capacity) dfa_table_grow(dfatable);

  DFATableEntry* entry = malloc(sizeof(DFATableEntry));
  entry->nfa_states = nfa_states;
  memset(entry->char_to_id, -1, sizeof(entry->char_to_id));
  entry->is_final = false;

  int new_id = dfatable->num_states++;
  dfatable->table[new_id] = entry;
  return new_id;
}

void dfa_table_destroy(DFATable* dfatable) {
  for (int i = 0; i < dfatable->num_states; ++i) {
    free(dfatable->table[i]->nfa_states);
    free(dfatable->table[i]);
  }
  free(dfatable->table);
  free(dfatable);
}
