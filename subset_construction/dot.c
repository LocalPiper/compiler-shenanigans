#include "dot.h"
#include <stdbool.h>
#include <stdio.h>

void emit_dfa(const DFATable *table, FILE *out) {
  int n = table->num_states;
  for (int i = 0; i < n; ++i) {
    DFATableEntry* entry = table->table[i];
    fprintf(out, "  n%d [shape=%s, label=\"n%d\"];\n", i, (entry->is_final)? "doublecircle" : "circle", i);
  }

  for (int i = 0; i < n; ++i) {
    DFATableEntry* entry = table->table[i];
    for (int c = 0; c < 256; ++c) {
      if (entry->char_to_id[c] != -1) {
        fprintf(out, "  n%d -> n%d [label=\"%c\"];\n", i, entry->char_to_id[c], (char)c);
      }
    }
  }
}

void write_dot(const DFATable *dfa, FILE *out) {
  fprintf(out, "digraph DFA {\n");
  fprintf(out, "  rankdir=LR;\n");

  emit_dfa(dfa, out);

  fprintf(out, "}\n");
}

void write_dot_file(const DFATable *dfa) {
  FILE *file = fopen("output.dot", "w+");
  if (!file) {
    perror("fopen()");
    return;
  }

  write_dot(dfa, file);
  fclose(file);
}
