#include "automaton.h"
#include "thompson.h"
#include "dot.h"
#include "dfa.h"
#include "dfa_table.h"
#include <stdio.h>
#include <stdlib.h>

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

