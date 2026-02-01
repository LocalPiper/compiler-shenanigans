#include "trie.h"
#include <stdlib.h>

void add_word(Trie* trie, const char* word) {
  Trie* curr = trie;
  const char* ptr = word;
  while (*ptr != '\0') {
    int c = *ptr - 'a';
    if (!curr->next[c]) {
      Trie* node = calloc(1, sizeof(Trie));
      curr->next[c] = node;
    }
    ++ptr;
    curr = curr->next[c];
  }
  curr->is_word = true;
}

void add_words(Trie* trie, const char** words) {
  for (const char** ptr = words; *ptr != NULL; ++ptr) {
    add_word(trie, *ptr);
  }
}

bool find_word(Trie* trie, const char* word) {
  Trie* curr = trie;
  const char* ptr = word;
  while (*ptr != '\0') {
    int c = *ptr - 'a';
    if (!curr->next[c]) return false;
    ++ptr;
    curr = curr->next[c];
  }
  return curr && curr->is_word;
}

void free_trie(Trie* trie) {
  if (!trie) return;
  for (int i = 0; i < ALPHABET_SIZE; ++i) {
    if (trie->next[i]) free_trie(trie->next[i]);
  }
  free(trie);
}
