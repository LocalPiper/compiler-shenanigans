#ifndef TRIE_H
#define TRIE_H

#include <stdbool.h>

#define ALPHABET_SIZE 26

typedef struct AC_DFA {
  struct AC_DFA* next[ALPHABET_SIZE];
  bool is_word;
  struct AC_DFA* suffix_link;
  struct AC_DFA* output_link;
  int word_id;
} AC_DFA;

void add_word(AC_DFA* dfa, const char* word, int id);
void add_words(AC_DFA* dfa, const char** words);
bool find_word(AC_DFA* dfa, const char* word);
void build_suffix_links(AC_DFA* dfa);
void build_output_links(AC_DFA* dfa);
void free_dfa(AC_DFA* dfa);

#endif // !TRIE_H
