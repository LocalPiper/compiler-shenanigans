#ifndef NFA_H
#define NFA_H

typedef struct State {
  int c;
  struct State* out1;
  struct State* out2;
  int id;
} State;

typedef struct Frag {
  State* start;
  State* out;
} Frag;

State* state_init(int c, State* out1, State* out2);
void nfa_destroy(State* nfa);

#endif // NFA_H
