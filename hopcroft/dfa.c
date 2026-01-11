#include <stdbool.h>
#include <stdio.h>


#define MAX_OUTPUTS 10

typedef struct State State;

struct State {
  int id;
  bool is_accepting;
  State* next[MAX_OUTPUTS];
};



