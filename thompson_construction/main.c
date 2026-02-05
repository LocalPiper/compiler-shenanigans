#include "nfa.h"
#include "dot.h"
#include "thompson.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <regex>\n", argv[0]);
    return 1;
  }

  char *regex = argv[1];
  State **nfa = thompson_construction(regex);
  
  write_dot_file(nfa[0]);

  nfa_destroy(nfa[0]);
  free(nfa);
  return 0;
}
