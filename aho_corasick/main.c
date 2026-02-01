#include "trie.h"
#include "dot.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef struct FreqMap {
  const char* s;
  int c;
} FreqMap;

static FreqMap* mp;

void find_all_matches(const AC_DFA* dfa, const char* text) {
  const AC_DFA* u = dfa;

  for (const char* p = text; *p; ++p) {
    int c = *p - 'a';

    while (u != dfa && !u->next[c]) u = u->suffix_link;
    if (u->next[c]) u = u->next[c];

    const AC_DFA* v = u;
    while (v) {
      if (v->is_word) mp[v->word_id].c++;
      v = v->output_link;
    }
  }
}

int count_all_matches(const AC_DFA* dfa, const char* text) {
  int res = 0;
  const AC_DFA* u = dfa;

  for (const char* p = text; *p; ++p) {
    int c = *p - 'a';

    while (u != dfa && !u->next[c]) u = u->suffix_link;
    if (u->next[c]) u = u->next[c];

    const AC_DFA* v = u;
    while (v) {
      if (v->is_word) ++res;
      v = v->output_link;
    }
  }

  return res;
}

int main(int argc, char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <pattern> ... <pattern> <text>\n", argv[0]);
    return 1;
  }

  int n = argc - 2;

  const char** patterns = malloc(n * sizeof(const char*));
  mp = calloc(n, sizeof(FreqMap));

  for (int i = 0; i < n; ++i) {
    patterns[i] = mp[i].s = argv[i + 1];
  }

  const char* text = argv[argc - 1];
  

  AC_DFA* dfa = calloc(1, sizeof(AC_DFA));
  dfa->word_id = -1;

  add_words(dfa, patterns);
  build_suffix_links(dfa);
  build_output_links(dfa);

  printf("total matches: %d\n", count_all_matches(dfa, text));

  find_all_matches(dfa, text);

  for (int i = 0; i < n; ++i) {
    printf("word: %s, matched: %d\n", mp[i].s, mp[i].c);
  }

  write_dot_file(dfa, false);

  free_dfa(dfa);
  free(patterns);
  free(mp);
}
