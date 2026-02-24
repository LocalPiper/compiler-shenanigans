#ifndef BITSET_H
#define BITSET_H
#include <stdbool.h>
#include <stdint.h>
#define MAX_NFA_STATES 100
#define WORD_BITS 64
#define BITSET_WORDS ((MAX_NFA_STATES + WORD_BITS - 1) / WORD_BITS)

typedef struct StateSet {
  uint64_t bits[BITSET_WORDS];
} StateSet;

bool stateset_equal(const StateSet* a, const StateSet* b) {
  for (int i = 0; i < BITSET_WORDS; ++i) {
    if (a->bits[i] != b->bits[i]) return false;
  }
  return true;
}

static inline bool stateset_contains(const StateSet* s, int id) {
  return (s->bits[id / 64] >> (id % 64)) & 1;
}

static inline void stateset_add(StateSet* s, int id) {
  s->bits[id / 64] |= (1ULL << (id %64));
}

#endif // BITSET_H
