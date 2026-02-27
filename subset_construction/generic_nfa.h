#ifndef GENERIC_NFA_H
#define GENERIC_NFA_H
#include <stdbool.h>

typedef struct GenericState {
  int id;
  bool is_final;
  struct Edge* edges;
} GenericState;

typedef struct Edge {
  int label;
  int target_id;
  struct Edge* next;
} Edge;

void generic_graph_destroy(GenericState* nfa, int num_states);

#endif // GENERIC_NFA_H

