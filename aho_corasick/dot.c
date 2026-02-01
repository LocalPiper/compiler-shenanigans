#include "dot.h"
#include <stdio.h>
#include <stdlib.h>

typedef struct NodeMap {
  const AC_DFA* node;
  int id;
} NodeMap;

static NodeMap* node_map;
static int map_size = 0;

static int next_node_id = 0;
static bool is_verbose = false;

void collect_nodes(const AC_DFA* node, int id) {
  if (!node) return;

  node_map[map_size].node = node;
  node_map[map_size].id = id;
  map_size++;

  for (int i = 0; i < ALPHABET_SIZE; ++i) {
    if (node->next[i]) {
      int child_id = next_node_id++;
      collect_nodes(node->next[i], child_id);
    }
  }
}

int get_node_id(const AC_DFA* node) {
  for (int i = 0; i < map_size; ++i) {
    if (node_map[i].node == node) return node_map[i].id;
  }
  return -1;
}

void emit_trie(FILE* out) {
  for (int i = 0; i < map_size; ++i) {
    fprintf(out, "  n%d [shape=%s];\n", 
            node_map[i].id, 
            node_map[i].node->is_word ? "doublecircle" : "circle");
  }

  for (int i = 0; i < map_size; ++i) {
    const AC_DFA* node = node_map[i].node;
    for (int j = 0; j < ALPHABET_SIZE; ++j) {
      if (node->next[j]) {
        fprintf(out, "  n%d -> n%d [label=\"%c\"];\n", 
                node_map[i].id, 
                get_node_id(node->next[j]),
                j + 'a');
      }
    }
    fprintf(out, "  n%d -> n%d [color=red];\n",
            node_map[i].id,
            get_node_id(node->suffix_link));
    if (node->output_link) fprintf(out, "  n%d -> n%d [color=blue];\n",
            node_map[i].id,
            get_node_id(node->output_link));
  }
}

void write_dot(const AC_DFA* dfa, FILE* out) {
  fprintf(out, "digraph DFA {\n");
  fprintf(out, "  rankdir=LR;\n");
  fprintf(out, "  node [label=\"\"]");

  int root_id = next_node_id++;
  node_map = malloc(100 * sizeof(NodeMap));
  collect_nodes(dfa, root_id);

  emit_trie(out);

  fprintf(out, "}\n");

  free(node_map);
}

void write_dot_file(const AC_DFA* dfa, bool verbose) {
  FILE* file = fopen("output.dot", "w+");
  is_verbose = verbose;
  if (!file) {
    perror("fopen()");
    return;
  }
  write_dot(dfa, file);
}
