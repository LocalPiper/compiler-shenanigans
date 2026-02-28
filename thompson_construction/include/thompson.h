#ifndef THOMPSON_H
#define THOMPSON_H
#include "automaton.h"

AutomatonNode* thompson_construction(char *s, int* num_states_out, int* start_out, int* end_out);

#endif // THOMPSON_H
