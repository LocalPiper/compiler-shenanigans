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

static int gstate_count = 0;

static int new_state(GenericState* gstates) {
  int id = gstate_count++;
  gstates[id].id = id;
  gstates[id].is_final = false;
  gstates[id].edges = NULL;
  return id;
}

static Fluke* new_fluke(int* in1, int* in2) {
  Fluke* fluke = calloc(1, sizeof(Fluke));
  fluke->in1 = (in1)? *in1 : -1;
  fluke->in2 = (in2)? *in2 : -1;
  return fluke;
}

static void add_egde(GenericState* gstates, int id, int label, int target_id) {
  Edge *e = calloc(1, sizeof(Edge));
  e->label = label;
  e->target_id = target_id;
  e->next = gstates[id].edges;
  gstates[id].edges = e;
}

static void replace_fluke(GenericState* gstates, Fluke* old_end, int new_end) {
  // flukes may contain ids of nodes that point to them (thompson guarantees no more than 2)
  // thompson also guarantees that any node has no more than 2 outgoing edges
  // check for backreferences and remap edges
  if (old_end->in1 != -1) {
    GenericState* state = &gstates[old_end->in1];
    for (Edge* e = state->edges; e != NULL; e = e->next) {
      if (e->target_id == -1) {
        e->target_id = new_end; break;
      }
    }
  }
  if (old_end->in2 != -1) {
    GenericState* state = &gstates[old_end->in2];
    for (Edge* e = state->edges; e != NULL; e = e->next) {
      if (e->target_id == -1) {
        e->target_id = new_end; break;
      }
    }
  }
  // fluke is no longer needed
  free(old_end);
}

GenericState* thompson(const char *s, int* num_states_out, int* start_out, int* end_out) {
  GenericState* gstates = calloc(1024, sizeof(GenericState));
  gstate_count = 0;
  fsp = 0;

  for (const char *p = s; *p; ++p) {
    char tok = *p;
    if (is_literal(tok)) {
      int start = new_state(gstates);
      Fluke* end = new_fluke(&start, NULL);
      add_egde(gstates, start, tok, -1); // have to add edge to nowhere in order to save token data
      push((Frag){start, end});
    } else if (tok == '.') {
      Frag f2 = pop();
      Frag f1 = pop();
      replace_fluke(gstates, f1.out, f2.in);
      push((Frag){f1.in, f2.out});
    } else if (tok == '|') {
      Frag f2 = pop();
      Frag f1 = pop();
      int split = new_state(gstates);
      add_egde(gstates, split, EPSILON, f1.in);
      add_egde(gstates, split, EPSILON, f2.in);
      int end1 = new_state(gstates);
      int end2 = new_state(gstates);
      replace_fluke(gstates, f1.out, end1);
      replace_fluke(gstates, f2.out, end2);
      add_egde(gstates, end1, EPSILON, -1);
      add_egde(gstates, end2, EPSILON, -1);
      Fluke* join = new_fluke(&end1, &end2);
      push((Frag){split, join});
    } else if (tok == '*') {
      Frag f = pop();
      int split = new_state(gstates);
      int end = new_state(gstates);
      replace_fluke(gstates,f.out, end);
      add_egde(gstates, split, EPSILON, f.in);
      add_egde(gstates, split, EPSILON, -1);
      add_egde(gstates, end, EPSILON, f.in);
      add_egde(gstates, end, EPSILON, -1);
      Fluke* join = new_fluke(&split, &end);
      push((Frag){split, join});
    }
  }
  Frag final_frag = pop();
  int end = new_state(gstates);
  replace_fluke(gstates, final_frag.out, end);
  gstates[end].is_final = true;

  if (num_states_out) *num_states_out = gstate_count;
  if (start_out) *start_out = final_frag.in;
  if (end_out) *end_out = end;
  return gstates;
}

GenericState* thompson_construction(char *s, int* num_states_out, int* start_out, int* end_out) {
  if (!strlen(s)) return NULL;
  explicit_concatenation(&s);
  shunting_yard(&s);
  GenericState* nfa = thompson(s, num_states_out, start_out, end_out);
  free(s);
  return nfa;
}
