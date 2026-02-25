#ifndef DFA_TABLE
#define DFA_TABLE
#include "../nfa_simulation/bitset.h"
#include <stdlib.h>
#include <string.h>

typedef struct DFATableEntry {
  StateSet* nfa_states;
  int char_to_id[256];
  bool is_final;
} DFATableEntry;

typedef struct {
  DFATableEntry** table;
  int num_states;
  int capacity;
} DFATable;

DFATable* dfa_table_init();
int dfa_table_find_state(DFATable* dfatable, StateSet* nfa_states);
int dfa_table_add_state(DFATable* dfatable, StateSet* nfa_states);
void dfa_table_destroy(DFATable* dfatable);

#endif // DFA_TABLE
