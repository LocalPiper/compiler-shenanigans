#ifndef DFA_H
#define DFA_H
#include "dfa_table.h"
#include "automaton.h"

DFATable* encode_new_states(AutomatonNode* nfa, int start_id, int num_states);
AutomatonNode* dfa_table_to_dfa(DFATable* table, int* num_states_out);

#endif // DFA_H
