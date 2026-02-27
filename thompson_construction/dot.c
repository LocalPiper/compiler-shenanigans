#include "dot.h"
#include <stdbool.h>
#include <stdio.h>

static char label_buf[2];

const char *get_label(int c) {
  if (c == -1)
    return "ε";
  if (c > 0) {
    label_buf[0] = (char)c;
    label_buf[1] = 0;
    return label_buf;
  }
  return NULL;
}

void emit_nfa(const GenericState *nfa, int num_states, FILE *out) {
  for (int i = 0; i < num_states; ++i) {
    fprintf(out, "  n%d [shape=%s, label=\"n%d\"];\n", nfa[i].id, nfa[i].is_final? "doublecircle" : "circle", nfa[i].id);
  }

  for (int i = 0; i < num_states; ++i) {
    Edge* e = nfa[i].edges;
    while (e) {
      Edge* next = e->next;
      const char *label = get_label(e->label);
      fprintf(out, "  n%d -> n%d [label=\"%s\"];\n", nfa[i].id, e->target_id, label);
      e = next;
    }
  }
}

void write_dot(const GenericState *nfa, int num_states, FILE *out) {
  fprintf(out, "digraph DFA {\n");
  fprintf(out, "  rankdir=LR;\n");

  emit_nfa(nfa, num_states, out);

  fprintf(out, "}\n");
}

void write_dot_file(const GenericState *nfa, int num_states) {
  FILE *file = fopen("output.dot", "w+");
  if (!file) {
    perror("fopen()");
    return;
  }

  write_dot(nfa, num_states, file);
  fclose(file);
}
