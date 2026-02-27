#include "dot.h"
#include "thompson.h"
#include "../subset_construction/generic_nfa.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <regex>\n", argv[0]);
    return 1;
  }

  char *regex = argv[1];
  int n = 0;
  GenericState *nfa = thompson_construction(regex, &n, NULL, NULL);
  
  write_dot_file(nfa, n);

  for (int i = 0; i < 1024; ++i) {
    Edge* e = nfa[i].edges;
    while (e) {
      Edge* next = e->next;
      free(e);
      e = next;
    }
  }
  free(nfa);

  return 0;
}
