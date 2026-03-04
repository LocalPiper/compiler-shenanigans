#include "syntax_tree.h"
#include "stack.h"
#include "thompson.h"
#include <string.h>
#include <stdlib.h>

SyntaxTreeNode* construct_syntax_tree(const char* rpn_regex, int* root_idx_out, int* num_nodes_out, int* num_literals_out) {
    int n = strlen(rpn_regex);
    SyntaxTreeNode* tree = calloc(n + 2, sizeof(SyntaxTreeNode));
    Stack* operand_stack = stack_init();
    int node_cnt = 0;
    int lit_cnt = 0;
    int i;

    for (i = 0; i < n; i++) {
        char c = rpn_regex[i];
        SyntaxTreeNode* node = &tree[node_cnt];
        node->left = node->right = node->id = -1;
        node->value = c;

        if (is_literal(c)) {
            node->id = lit_cnt++;
        } else {
            int lhs_idx = -1, rhs_idx = -1;
            
            if (c == '*') {
                lhs_idx = stack_pop(operand_stack);
            } else {
                rhs_idx = stack_pop(operand_stack);
                lhs_idx = stack_pop(operand_stack);
            }
            
            node->left = lhs_idx;
            node->right = rhs_idx;
        }
        stack_push(operand_stack, node_cnt);
        node_cnt++;
    }

    *root_idx_out = stack_pop(operand_stack); 

    *num_nodes_out = node_cnt;
    *num_literals_out = lit_cnt;
    stack_destroy(operand_stack);
    return tree;
}

SyntaxTreeNode* construct_augmented_syntax_tree(const char* rpn, int* root_idx_out, int* num_nodes_out, int* num_literals_out) {
    int orig_root, orig_nodes, orig_lits;
    SyntaxTreeNode* tree = construct_syntax_tree(rpn, &orig_root, &orig_nodes, &orig_lits);
    
    int concat_idx = orig_nodes + 1;
    tree[concat_idx].value = '.';
    tree[concat_idx].left = orig_root;
    tree[concat_idx].id = -1;
    
    int end_idx = orig_nodes;
    tree[end_idx].value = '#';
    tree[end_idx].id = orig_lits++;
    tree[end_idx].left = tree[end_idx].right = -1;
    
    tree[concat_idx].right = end_idx;
    
    *root_idx_out = concat_idx;
    *num_nodes_out = orig_nodes + 2;
    *num_literals_out = orig_lits;
    return tree;
}

