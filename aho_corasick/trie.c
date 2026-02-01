#include "trie.h"
#include <stdlib.h>

void add_word(AC_DFA* dfa, const char* word, int id) {
  AC_DFA* curr = dfa;
  const char* ptr = word;
  while (*ptr != '\0') {
    int c = *ptr - 'a';
    if (!curr->next[c]) {
      AC_DFA* node = calloc(1, sizeof(AC_DFA));
      node->word_id = -1;
      curr->next[c] = node;
    }
    ++ptr;
    curr = curr->next[c];
  }
  curr->is_word = true;
  curr->word_id = id;
}

void add_words(AC_DFA* dfa, const char** words) {
  int i = 0;
  for (const char** ptr = words; *ptr != NULL; ++ptr) {
    add_word(dfa, *ptr, i++);
  } 
}

bool find_word(AC_DFA* dfa, const char* word) {
  AC_DFA* curr = dfa;
  const char* ptr = word;
  while (*ptr != '\0') {
    int c = *ptr - 'a';
    if (!curr->next[c]) return false;
    ++ptr;
  curr = curr->next[c];
  }
  return curr && curr->is_word;
}

void build_suffix_links(AC_DFA* dfa) {
  if (!dfa) return;
  dfa->suffix_link = dfa;

  AC_DFA* queue[1000] = {0};
  int l = 0, r = 0;

  for (int i = 0; i < ALPHABET_SIZE; ++i) {
    if (dfa->next[i]) {
      dfa->next[i]->suffix_link = dfa;
      queue[r++] = dfa->next[i];
    }
  }

  while (l < r) {
    AC_DFA* w = queue[l++];

    for (int i = 0; i < ALPHABET_SIZE; ++i) {
      if (w->next[i]) {
        AC_DFA* x = w->suffix_link;
        while (x != dfa && !x->next[i]) x = x->suffix_link;

        w->next[i]->suffix_link = x->next[i]? x->next[i] : dfa;
        queue[r++] = w->next[i];
      }
    }
  }
}


void build_output_links(AC_DFA* dfa) {
  AC_DFA* queue[1000] = {0};
  int l = 0, r = 0;
  queue[r++] = dfa;

  while (l < r) {
    AC_DFA* w = queue[l++];

    for (int i = 0; i < ALPHABET_SIZE; ++i) {
      if (w->next[i]) {
        queue[r++] = w->next[i];
      }
    }

    if (w != dfa) {
      AC_DFA* x = w->suffix_link;
      while (x != dfa && !x->is_word) x = x->suffix_link;
      if (x != dfa) w->output_link = x;
    }
  }
}

void free_dfa(AC_DFA* dfa) {
  if (!dfa) return;
  for (int i = 0; i < ALPHABET_SIZE; ++i) {
    if (dfa->next[i]) free_dfa(dfa->next[i]);
  }
  free(dfa);
}
