#include "stack.h"
#include "nfa.h"
#include "dot.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#define EPSILON -1
#define MATCH -2

int is_literal(const char c) {
  return c != '(' && c != ')' && c != '*' && c != '|' && c != '.';
}

int concatenatable(char a, char b) {
  int r1 = is_literal(a), r2 = is_literal(b);
  return (r1 && r2) 
      || (r1 && b == '(') 
      || (a == ')' && r2) 
      || (a == ')' && b == '(') 
      || (a == '*' && r2) 
      || (a == '*' && b == '(');
}

void explicit_concatenation(char** ps) {
  char* s = *ps;
  size_t n = strlen(s);
  char* res = malloc(n * 2 + 1);
  
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
  if (c == '*') return 3;
  if (c == '.') return 2;
  if (c == '|') return 1;
  return 0;
}

void shunting_yard(char** ps) {
  char* s = *ps;
  size_t n = strlen(s);
  char* res = malloc(n + 1);
  size_t j = 0;

  Stack* ops = stack_init();
  for (size_t i = 0; i < n; ++i) {
    char c = s[i];

    if (is_literal(c)) {
      res[j++] = c;
    } else if (c == '(') {
      stack_push(ops, c);
    } else if (c == ')') {
      while (!stack_empty(ops) && stack_top(ops) != '(') res[j++] = stack_pop(ops);
      if (!stack_empty(ops)) stack_pop(ops);
    } else {
      while (!stack_empty(ops) && 
             stack_top(ops) != '(' && 
             get_precedence(c) <= get_precedence(stack_top(ops))) {
        res[j++] = stack_pop(ops);
      }
      stack_push(ops, c);
    }
  }

  while (!stack_empty(ops)) res[j++] = stack_pop(ops);
  res[j] = '\0';

  free(s);
  stack_destroy(ops);
  *ps = res;
}

Frag frag_stack[1024];
int fsp = 0;

void push(Frag f) { frag_stack[fsp++] = f; }
Frag pop() { return frag_stack[--fsp]; }

State* thompson(const char* s) {
  fsp = 0;
  for (const char* p = s; *p; ++p) {
    char tok = *p;
    if (is_literal(tok)) {
      State* start = state_init(tok, NULL, NULL);
      State* accept = state_init(0, NULL, NULL);
      start->out1 = accept;
      push((Frag){start, accept});
    } else if (tok == '.') {
      Frag f2 = pop();
      Frag f1 = pop();
      f1.out->out1 = f2.start->out1;
      f1.out->out2 = f2.start->out2;
      f1.out->c = f2.start->c;
      push((Frag){f1.start, f2.out});
    } else if (tok == '|') {
      Frag f2 = pop();
      Frag f1 = pop();
      State* split = state_init(EPSILON, f1.start, f2.start);
      State* join = state_init(0, NULL, NULL);
      f1.out->c = EPSILON;
      f1.out->out1 = join;
      f2.out->c = EPSILON;
      f2.out->out1 = join;
      push((Frag){split, join});
    } else if (tok == '*') {
      Frag f = pop();
      State* join = state_init(0, NULL, NULL);
      State* split = state_init(EPSILON, f.start, join);
      f.out->c = EPSILON;
      f.out->out1 = f.start;
      f.out->out2 = join;
      push((Frag){split, join});
    }
  }
  Frag final_frag = pop();
  final_frag.out->is_final = true;
  return final_frag.start;
}
void number_states(State* nfa) {
  int id = 0;
  State* state_stack[1024];
  int sp = 0;

  state_stack[sp++] = nfa;

  while (sp) {
    State* s = state_stack[--sp];
    if (s->id != -1) continue;

    s->id = id++;

    if (s->out1) state_stack[sp++] = s->out1;
    if (s->out2) state_stack[sp++] = s->out2; 
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <regex>\n", argv[0]);
    return 1;
  }
  char* regex = argv[1];
  explicit_concatenation(&regex);
  printf("%s\n", regex);
  shunting_yard(&regex);
  printf("%s\n", regex);
  State* nfa = thompson(regex);
  number_states(nfa);
  write_dot_file(nfa);
  free(regex);
  return 0;
}
