#include "direct.h"
#include "thompson.h"
#include "syntax_tree.h"
#include "bitset.h"
#include "automaton.h"
#include "dfa_table.h"
#include "dfa.h"
#include <stdlib.h>

int* get_literals_array(SyntaxTreeNode* tree, int num_nodes, int num_literals) {
  int* index_to_id = calloc(num_literals, sizeof(int));
  for (int i = 0; i < num_nodes; ++i) {
    if (tree[i].id != -1) {
      index_to_id[tree[i].id] = i;
    }
  }
  return index_to_id;
}

void update_followpos(StateSet* followpos_idxs, StateSet* from, StateSet* followpos) {
  for (int word = 0; word < BITSET_WORDS; ++word) {
    uint64_t bits = followpos_idxs->bits[word];
    while (bits != 0) {
      int bitpos = __builtin_ctzll(bits);
      int state_id = word * 64 + bitpos;
      stateset_update(&followpos[state_id], from);
      bits ^= (1LL << bitpos);
    }
  }
}

void fill_node_sets(SyntaxTreeNode* tree, StateSet* followpos, int n) {
  for (int i = 0; i < n; ++i) {
    if (tree[i].id != -1) {
      tree[i].nullable = false;
      stateset_add(&tree[i].firstpos, tree[i].id);
      stateset_add(&tree[i].lastpos, tree[i].id);
    } else if (tree[i].value == '*') {
      tree[i].nullable = true;
      stateset_update(&tree[i].firstpos, &tree[i - 1].firstpos);
      stateset_update(&tree[i].lastpos, &tree[i - 1].lastpos);
      update_followpos(&tree[i].lastpos, &tree[i].firstpos, followpos);
    } else if (tree[i].value == '|') {
      tree[i].nullable = tree[i - 2].nullable || tree[i - 1].nullable;
      stateset_update(&tree[i].firstpos, &tree[i - 1].firstpos);
      stateset_update(&tree[i].firstpos, &tree[i - 2].firstpos);
      stateset_update(&tree[i].lastpos, &tree[i - 1].lastpos);
      stateset_update(&tree[i].lastpos, &tree[i - 2].lastpos);
    } else if (tree[i].value == '.') {
      tree[i].nullable = tree[i - 2].nullable && tree[i - 1].nullable;
      stateset_update(&tree[i].firstpos, &tree[i - 2].firstpos);
      if (tree[i - 2].nullable) {
        stateset_update(&tree[i].firstpos, &tree[i - 1].firstpos);
      }
      stateset_update(&tree[i].lastpos, &tree[i - 1].lastpos);
      if (tree[i - 1].nullable) {
        stateset_update(&tree[i].lastpos, &tree[i - 2].lastpos);
      }
      update_followpos(&tree[i - 2].lastpos, &tree[i - 1].firstpos, followpos);
    }
  }
}

DFATable* tree_to_dfa_table(SyntaxTreeNode* root, int* index_to_id, int start_idx, int final_idx, StateSet* followpos) {
  DFATable* table = dfa_table_init();
  StateSet* S = calloc(1, sizeof(StateSet));
  stateset_update(S, &root[start_idx].firstpos);
  dfa_table_add_state(table, S);
  if (stateset_contains(S, root[final_idx].id)) table->table[0]->is_final = true;
  int tp = 0;
  int unchecked = 1;

  while (tp < unchecked) {
    DFATableEntry* entry = table->table[tp++];
    for (int c = 0; c < 256; ++c) {
      StateSet* U = calloc(1, sizeof(StateSet));
      for (int word = 0; word < BITSET_WORDS; ++word) {
        uint64_t bits = entry->nfa_states->bits[word];
        while (bits != 0) {
          int bitpos = __builtin_ctzll(bits);
          int state_id = word * 64 + bitpos;
          if (root[index_to_id[state_id]].value == (char)c) stateset_update(U, &followpos[state_id]);
          bits ^= (1LL << bitpos);
        }
      }
      int state_id = dfa_table_find_state(table, U);
      if (state_id == -1 && !stateset_empty(U)) {
        int new_id = dfa_table_add_state(table, U);
        if (stateset_contains(U, root[final_idx].id)) table->table[new_id]->is_final = true; 
        entry->char_to_id[c] = unchecked++;
      } else {
        if (state_id != -1) entry->char_to_id[c] = state_id; 
        free(U);
      }
    }
  }
  return table;
}

AutomatonNode* fast_dfa(char* regex, int* n_out) {
  explicit_concatenation(&regex);
  shunting_yard(&regex);

  int root_idx = 0, nodes = 0, literal_nodes = 0;
  SyntaxTreeNode* tree = construct_augmented_syntax_tree(regex, &root_idx, &nodes, &literal_nodes);

  StateSet* followpos = calloc(literal_nodes, sizeof(StateSet));
  fill_node_sets(tree, followpos, nodes);

  int* index_to_id = get_literals_array(tree, nodes, literal_nodes);
  DFATable* table = tree_to_dfa_table(tree, index_to_id, root_idx, root_idx - 1, followpos);
  AutomatonNode* dfa = dfa_table_to_dfa(table, n_out);

  free(tree);
  free(followpos);
  free(index_to_id);
  dfa_table_destroy(table);

  return dfa;
}

