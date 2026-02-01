#include "trie.h"
#include "dot.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

bool verbose = false;

int main(int argc, char** argv) {
  if (argc >= 2) {
    if (strcmp(argv[1], "-v") == 0) {
      verbose = true;
      printf("%d", verbose);
    } else {
      printf("Usage: ./a.out [-v]\n");
      printf("-v - verbose printer of trie\n");
      return -1;
    }
  }

  Trie* trie = calloc(1, sizeof(Trie));

  const char* words[] = {"all", "tall", "fall", "fatal", "llama", "lame", NULL};
  add_words(trie, words);

  write_dot_file(trie, verbose);

  free_trie(trie);
  return 0;
}
