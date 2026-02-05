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
