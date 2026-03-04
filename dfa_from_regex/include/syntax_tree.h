#ifndef SYNTAX_TREE_H
#define SYNTAX_TREE_H
#include "bitset.h"

typedef struct SyntaxTreeNode {
  int id; // only for leafs
  char value;
  int left, right;
  bool nullable;
  StateSet firstpos, lastpos;
} SyntaxTreeNode;

SyntaxTreeNode* construct_syntax_tree(const char* rpn_regex, int* root_idx_out, int* num_nodes_out, int* num_literals_out);
SyntaxTreeNode* construct_augmented_syntax_tree(const char* rpn, int* root_idx_out, int* num_nodes_out, int* num_literals_out);

#endif // SYNTAX_TREE_H
