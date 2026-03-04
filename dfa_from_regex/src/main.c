#include "automaton.h"
#include "direct.h"
#include "dot.h"
#include <stdio.h>
#include <string.h>

bool simulate(AutomatonNode* dfa, char* word) {
  AutomatonNode* curr = &dfa[0];
  for (char* p = word; *p; ++p) {
    AutomatonEdge* e = curr->edges;
    bool b = false;
    while (e) {
      if (e->label == *p) {
        b = true;
        curr = &dfa[e->target_id];
        break;
      }
      e = e->next;
    }
    if (!b) return false;
  }
  return curr->is_final;
}

int main(int argc, char** argv) {
  if (argc < 3) {
    fprintf(stderr, "Usage: %s <regex> <string> [<string>...]\n", argv[0]);
    return 1;
  }
  char* regex = argv[1];
  int n = argc - 2;
  char** words = argv + 2;
  
  if (!strlen(regex)) {
    fprintf(stderr, "Provided empty regex\n");
    return 1;
  }

  int num_nodes = 0;
  AutomatonNode* dfa = fast_dfa(regex, &num_nodes);

  write_dot_file(dfa, num_nodes);

  for (int i = 0; i < n; ++i) {
    printf("%s %d\n", words[i], simulate(dfa, words[i]));
  }

  automaton_destroy(dfa, num_nodes);
}
