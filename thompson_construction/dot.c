#include "nfa.h"
#include <stdio.h>
#include <stdbool.h>

static char label_buf[2];

const char* get_label(int c) {
  if (c == -1) return "ε";
  if (c > 0) {
    label_buf[0] = (char)c;
    label_buf[1] = 0;
    return label_buf;
  }
  return NULL;
}

void emit_nfa(const State* s, FILE* out, bool* vis) {
  if (!s || vis[s->id]) return;
  vis[s->id] = true;

  const char* shape = s->is_final ? "doublecircle" : "circle";
  fprintf(out, "  n%d [shape=%s, label=\"n%d\"];\n", s->id, shape, s->id);

  const char* label = get_label(s->c);

  if (s->out1 && label) {
    fprintf(out, "  n%d -> n%d [label=\"%s\"];\n", s->id, s->out1->id, label);
  }

  if (s->out2 && label) {
    fprintf(out, "  n%d -> n%d [label=\"%s\"];\n", s->id, s->out2->id, label);
  }

  emit_nfa(s->out1, out, vis);
  emit_nfa(s->out2, out, vis);
}


void write_dot(const State* nfa, FILE* out) {
  fprintf(out, "digraph DFA {\n");
  fprintf(out, "  rankdir=LR;\n");

  bool vis[1024] = {0};
  emit_nfa(nfa, out, vis);

  fprintf(out, "}\n");
}

void write_dot_file(const State* nfa) {
  FILE* file = fopen("output.dot", "w+");
  if (!file) {
    perror("fopen()");
    return;
  }

  write_dot(nfa, file);
  fclose(file);
}
