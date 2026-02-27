#ifndef AUTOMATON_H
#define AUTOMATON_H
#include "bitset.h"
#include <stdbool.h>

typedef struct AutomatonNode {
  int id;
  bool is_final;
  struct AutomatonEdge* edges;
} AutomatonNode;

typedef struct AutomatonEdge {
  int label;
  int target_id;
  struct AutomatonEdge* next;
} AutomatonEdge;


void automaton_destroy(AutomatonNode* G, int n);
void eps_closure(AutomatonNode* G, StateSet* S);
StateSet* move(AutomatonNode* G, StateSet* S, int c);

#endif // AUTOMATON_H
