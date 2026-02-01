#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#define ALPHABET_SIZE 26

typedef struct Trie {
  struct Trie *next[ALPHABET_SIZE];
  bool is_word;
} Trie;

void add_word(Trie* trie, const char* word);
void add_words(Trie* trie, const char** words);
bool find_word(Trie* trie, const char* word);
void free_trie(Trie* trie);
#endif // TRIE_H
