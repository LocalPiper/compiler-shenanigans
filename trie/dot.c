#include "dot.h"
#include <stdio.h>

static int next_node_id = 0;
static bool is_verbose = false;

void emit_trie(const Trie* node, FILE* out, int id, const char* path_so_far) {
  if (!node) return;
  fprintf(out, "  n%d [label=\"%s\", shape=%s];\n", id, path_so_far, node->is_word ? "doublecircle" : "circle");

  char path[512];
  for (int c = 0; c < ALPHABET_SIZE; ++c) {
    if (node->next[c]) {
      int child_id = next_node_id++;
      char letter = c + 'a';

      if (is_verbose) {
        snprintf(path, sizeof(path), "%s%c", path_so_far, letter);
      }

      fprintf(out, "  n%d -> n%d [label=\"%c\"]\n", id, child_id, letter);

      emit_trie(node->next[c], out, child_id, path);
    }
  }
}

void write_dot(const Trie* trie, FILE* out) {
  fprintf(out, "digraph DFA {\n");
  fprintf(out, "  rankdir=LR;\n");
  fprintf(out, "  node [label=\"\"]");

  int root_id = next_node_id++;

  emit_trie(trie, out, root_id, "");

  fprintf(out, "}\n");
}

void write_dot_file(const Trie* trie, bool verbose) {
  FILE* file = fopen("output.dot", "w+");
  is_verbose = verbose;
  if (!file) {
    perror("fopen()");
    return;
  }
  write_dot(trie, file);
}
