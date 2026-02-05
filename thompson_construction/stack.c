#include "stack.h"
#include <stdlib.h>

Stack* stack_init() {
  int* arr = calloc(100, sizeof(int));
  Stack* st = calloc(1, sizeof(Stack));
  st->arr = arr;
  st->n = 0;
  st->cap = 100;
  return st;
}

int stack_empty(const Stack* st) {
  return st->n == 0;
}

void stack_grow(Stack* st) {
  st->arr = (int*)realloc(st->arr, st->cap * 2);
  st->cap *= 2;
}

void stack_push(Stack* st, int c) {
  if (st->n + 1 == st->cap) stack_grow(st);
  st->arr[st->n++] = c;
}

int stack_top(Stack* st) {
  return st->arr[st->n - 1];
} 

int stack_pop(Stack* st) {
  if (st->n - 1 < 0) return -128;
  return st->arr[--st->n];
}

void stack_destroy(Stack* st) {
  free(st->arr);
  free(st);
}
