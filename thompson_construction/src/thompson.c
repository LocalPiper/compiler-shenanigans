#include "thompson.h"
#include "stack.h"
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define EPSILON -1
#define MATCH -2

int is_literal(const char c) {
  return c != '(' && c != ')' && c != '*' && c != '|' && c != '.';
}

int concatenatable(char a, char b) {
  int r1 = is_literal(a), r2 = is_literal(b);
  return (r1 && r2) || (r1 && b == '(') || (a == ')' && r2) ||
         (a == ')' && b == '(') || (a == '*' && r2) || (a == '*' && b == '(');
}

void explicit_concatenation(char **ps) {
  char *s = *ps;
  size_t n = strlen(s);
  char *res = malloc(n * 2 + 1);

  size_t i = 0, j = 0;
  while (i < n) {
    res[j++] = s[i];
    if (i + 1 < n && concatenatable(s[i], s[i + 1])) {
      res[j++] = '.';
    }
    ++i;
  }

  res[j] = '\0';
  *ps = res;
}

int get_precedence(const char c) {
  if (c == '*')
    return 3;
  if (c == '.')
    return 2;
  if (c == '|')
    return 1;
  return 0;
}

void shunting_yard(char **ps) {
  char *s = *ps;
  size_t n = strlen(s);
  char *res = malloc(n + 1);
  size_t j = 0;

  Stack *ops = stack_init();
  for (size_t i = 0; i < n; ++i) {
    char c = s[i];

    if (is_literal(c)) {
      res[j++] = c;
    } else if (c == '(') {
      stack_push(ops, c);
    } else if (c == ')') {
      while (!stack_empty(ops) && stack_top(ops) != '(')
        res[j++] = stack_pop(ops);
      if (!stack_empty(ops))
        stack_pop(ops);
    } else {
      while (!stack_empty(ops) && stack_top(ops) != '(' &&
             get_precedence(c) <= get_precedence(stack_top(ops))) {
        res[j++] = stack_pop(ops);
      }
      stack_push(ops, c);
    }
  }

  while (!stack_empty(ops))
    res[j++] = stack_pop(ops);
  res[j] = '\0';

  free(s);
  stack_destroy(ops);
  *ps = res;
}

typedef struct Fluke {
  int in1;
  int in2;
} Fluke;

typedef struct Frag {
  int in;
  Fluke* out;
} Frag;

static Frag frag_stack[1024];
static int fsp = 0;

static void push(Frag f) { frag_stack[fsp++] = f; }
static Frag pop() { return frag_stack[--fsp]; }

static int state_count = 0;

static int new_state(AutomatonNode* states) {
  int id = state_count++;
  states[id].id = id;
  states[id].is_final = false;
  states[id].edges = NULL;
  return id;
}

static Fluke* new_fluke(int* in1, int* in2) {
  Fluke* fluke = calloc(1, sizeof(Fluke));
  fluke->in1 = (in1)? *in1 : -1;
  fluke->in2 = (in2)? *in2 : -1;
  return fluke;
}

static void add_egde(AutomatonNode* states, int id, int label, int target_id) {
  AutomatonEdge *e = calloc(1, sizeof(AutomatonEdge));
  e->label = label;
  e->target_id = target_id;
  e->next = states[id].edges;
  states[id].edges = e;
}

static void replace_fluke(AutomatonNode* states, Fluke* old_end, int new_end) {
  // flukes may contain ids of nodes that point to them (thompson guarantees no more than 2)
  // thompson also guarantees that any node has no more than 2 outgoing edges
  // check for backreferences and remap edges
  if (old_end->in1 != -1) {
    AutomatonNode* state = &states[old_end->in1];
    for (AutomatonEdge* e = state->edges; e != NULL; e = e->next) {
      if (e->target_id == -1) {
        e->target_id = new_end; break;
      }
    }
  }
  if (old_end->in2 != -1) {
    AutomatonNode* state = &states[old_end->in2];
    for (AutomatonEdge* e = state->edges; e != NULL; e = e->next) {
      if (e->target_id == -1) {
        e->target_id = new_end; break;
      }
    }
  }
  // fluke is no longer needed
  free(old_end);
}

AutomatonNode* thompson(const char *s, int* num_states_out, int* start_out, int* end_out) {
  AutomatonNode* states = calloc(1024, sizeof(AutomatonNode));
  state_count = 0;
  fsp = 0;

  for (const char *p = s; *p; ++p) {
    char tok = *p;
    if (is_literal(tok)) {
      int start = new_state(states);
      Fluke* end = new_fluke(&start, NULL);
      add_egde(states, start, tok, -1); // have to add edge to nowhere in order to save token data
      push((Frag){start, end});
    } else if (tok == '.') {
      Frag f2 = pop();
      Frag f1 = pop();
      replace_fluke(states, f1.out, f2.in);
      push((Frag){f1.in, f2.out});
    } else if (tok == '|') {
      Frag f2 = pop();
      Frag f1 = pop();
      int split = new_state(states);
      add_egde(states, split, EPSILON, f1.in);
      add_egde(states, split, EPSILON, f2.in);
      int end1 = new_state(states);
      int end2 = new_state(states);
      replace_fluke(states, f1.out, end1);
      replace_fluke(states, f2.out, end2);
      add_egde(states, end1, EPSILON, -1);
      add_egde(states, end2, EPSILON, -1);
      Fluke* join = new_fluke(&end1, &end2);
      push((Frag){split, join});
    } else if (tok == '*') {
      Frag f = pop();
      int split = new_state(states);
      int end = new_state(states);
      replace_fluke(states,f.out, end);
      add_egde(states, split, EPSILON, f.in);
      add_egde(states, split, EPSILON, -1);
      add_egde(states, end, EPSILON, f.in);
      add_egde(states, end, EPSILON, -1);
      Fluke* join = new_fluke(&split, &end);
      push((Frag){split, join});
    }
  }
  Frag final_frag = pop();
  int end = new_state(states);
  replace_fluke(states, final_frag.out, end);
  states[end].is_final = true;

  if (num_states_out) *num_states_out = state_count;
  if (start_out) *start_out = final_frag.in;
  if (end_out) *end_out = end;
  return states;
}

AutomatonNode* thompson_construction(char *s, int* num_states_out, int* start_out, int* end_out) {
  if (!strlen(s)) return NULL;
  explicit_concatenation(&s);
  shunting_yard(&s);
  AutomatonNode* nfa = thompson(s, num_states_out, start_out, end_out);
  free(s);
  return nfa;
}
