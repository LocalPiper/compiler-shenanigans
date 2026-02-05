#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int is_literal(const char c) {
  return c != '(' && c != ')' && c != '*' && c != '|';
}

int concatenatable(char a, char b) {
  int r1 = is_literal(a), r2 = is_literal(b);
  return (r1 && r2) 
      || (r1 && b == '(') 
      || (a == ')' && r2) 
      || (a == ')' && b == '(') 
      || (a == '*' && r2) 
      || (a == '*' && b == '(');
}

void explicit_concatenation(char** ps) {
  char* s = *ps;
  size_t n = strlen(s);
  char* res = malloc(n * 2 + 1);
  
  size_t i = 0, j = 0;
  while (i < n) {
    res[j++] = s[i];
    if (i + 1 < n && concatenatable(s[i], s[i + 1])) {
      res[j++] = '.';
    }
    ++i;
  }

  res[j] = '\0';
  *ps = res;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <regex>\n", argv[0]);
    return 1;
  }
  char* regex = argv[1];
  explicit_concatenation(&regex);
  printf("%s\n", regex);
  free(regex);
  return 0;
}
