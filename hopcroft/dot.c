#include "dot.h"
#include <stdio.h>


void write_dot(DFA* dfa, FILE* out) {
  State* states = dfa->states;
  fprintf(out, "digraph DFA {\n");
  fprintf(out, "  rankdir=LR;\n");
  fprintf(out, "  start [shape=point];\n");
  fprintf(out, "  start -> q%d;\n", states[dfa->initial].id);

  for (size_t i = 0; i < dfa->sz; ++i) {
    fprintf(out, "  q%d [shape=%s]\n", states[i].id, states[i].is_accepting? "doublecircle" : "circle");
  }

  for (size_t i = 0; i < dfa->sz; ++i) {
    for (int c = 0; c < 10; ++c) {
      if (states[i].next[c]) {
        fprintf(out, "  q%d -> q%d [label=\"%d\"]\n", states[i].id, states[states[i].next[c]].id, c);
      }
    }
  }

  fprintf(out, "}\n");
}

void write_dot_file(DFA* dfa) {
  FILE* file = fopen("output.dot", "w+");
  if (!file) {
    perror("fopen()");
    return;
  }
  write_dot(dfa, file);
}
