#include "thompson.h"
#include "automaton.h"
#include "bitset.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

bool simulate(AutomatonNode* gs, int start_id, int end_id, char* str) {
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
  AutomatonNode* nfa = thompson_construction(regex, &num_states, &start_id, &end_id);
  if (!nfa) {
    fprintf(stderr, "Provided empty regex\n");
    return 1;
  }
  for (int i = 0; i < n; ++i) {
    printf("%s %d\n", words[i], simulate(nfa, start_id, end_id, words[i]));
  }

  automaton_destroy(nfa, num_states);
}
