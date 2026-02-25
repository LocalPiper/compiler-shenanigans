#include "generic_nfa.h"
#include <stdlib.h>

void generic_graph_destroy(GenericState* nfa, int num_states) {
  if (!nfa) return;
  for (int i = 0; i < num_states; ++i) {
    Edge* e = nfa[i].edges;
    while (e) {
      Edge* next = e->next;
      free(e);
      e = next;
    }
  }
  free(nfa);
}



