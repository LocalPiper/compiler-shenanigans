#include "bitset.h"
#include <stdbool.h>
#define MAX_NFA_STATES 100
#define WORD_BITS 64
#define BITSET_WORDS ((MAX_NFA_STATES + WORD_BITS - 1) / WORD_BITS)

bool stateset_equal(const StateSet* a, const StateSet* b) {
  for (int i = 0; i < BITSET_WORDS; ++i) {
    if (a->bits[i] != b->bits[i]) return false;
  }
  return true;
}

bool stateset_check_intersection(const StateSet* a, const StateSet* b) {
  for (int i = 0; i < BITSET_WORDS; ++i) {
    if (a->bits[i] & b->bits[i]) return true;
  }
  return false;
}

bool stateset_empty(const StateSet* s) {
  for (int i = 0; i < BITSET_WORDS; ++i) {
    if (s->bits[i]) return false;
  }
  return true;
}

void stateset_update(StateSet* a, const StateSet* b) {
  for (int i = 0; i < BITSET_WORDS; ++i) {
    a->bits[i] |= b->bits[i];
  }
}
