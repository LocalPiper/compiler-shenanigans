#ifndef STACK_H
#define STACK_H
#include <stdlib.h>

typedef struct Stack {
  int* arr;
  size_t n;
  size_t cap;
} Stack;

Stack* stack_init();
void stack_push(Stack* st, int c);
int stack_top(Stack* st);
int stack_pop(Stack* st);
int stack_empty(const Stack* st);
void stack_destroy(Stack* st);

#endif // !STACK_H
