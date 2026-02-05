#include "nfa.h"
#include <stdlib.h>

State* state_init(int c, State* out1, State* out2) {
  State* s = calloc(1, sizeof(State));
  s->c = c;
  s->out1 = out1;
  s->out2 = out2;
  s->id = -1;
  return s;
}

void nfa_destroy(State* nfa) {
  if (!nfa) return;
  
  bool vis[1024] = {0};
  State* stack[1024];
  int sp = 0;
  
  stack[sp++] = nfa;
  
  while (sp) {
    State* s = stack[--sp];
    if (s->id >= 0 && vis[s->id]) continue;
    if (s->id >= 0) vis[s->id] = true;
    
    if (s->out1) stack[sp++] = s->out1;
    if (s->out2) stack[sp++] = s->out2;
  }
  
  stack[sp++] = nfa;
  while (sp) {
    free(stack[--sp]);
  }
}



