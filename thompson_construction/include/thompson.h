#ifndef THOMPSON_H
#define THOMPSON_H
#include "automaton.h"

int is_literal(const char c);
void explicit_concatenation(char **ps);
void shunting_yard(char **ps);
AutomatonNode* thompson_construction(char *s, int* num_states_out, int* start_out, int* end_out);

#endif // THOMPSON_H
