#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

int* f;

void compute_failure_function(const char* s) {
  int n = strlen(s);
  int* arr = calloc(n, sizeof(int));
  size_t i = 0;
  for (size_t j = 1; j < n; ++j) {
    while (i > 0 && s[i] != s[j]) i = arr[i - 1];
    if (s[i] == s[j]) ++i;
    arr[j] = i;
  }
  f = arr;
}

bool find(const char* s, const char* t) {
  size_t n = strlen(s);
  size_t m = strlen(t);
  size_t i = 0;

  for (size_t j = 0; j < m; ++j) {
    while (i > 0 && s[i] != t[j]) i = f[i - 1];
    if (s[i] == t[j]) ++i;
    if (i == n) return true;
  }

  return false;
}

int main(int argc, char** argv) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <pattern> <text>\n", argv[0]);
    return 1;
  }

  const char* s = argv[1];
  const char* t = argv[2];

  compute_failure_function(s);

  printf("%s\n", find(s, t)? "True" : "False");

  free(f);
}
