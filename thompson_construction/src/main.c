#include "dot.h"
#include "thompson.h"
#include "automaton.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <regex>\n", argv[0]);
    return 1;
  }

  char *regex = argv[1];
  int n = 0;
  AutomatonNode *nfa = thompson_construction(regex, &n, NULL, NULL);
  if (!nfa) return 0;
  write_dot_file(nfa, n);

  automaton_destroy(nfa, n);

  return 0;
}
