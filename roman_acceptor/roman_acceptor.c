#include <stdbool.h>
#include <stdio.h>
#include <string.h>

const char *input;
int position = 0;

bool match(const char *str) {
  int len = strlen(str);
  if (strncmp(input + position, str, len) == 0) {
    position += len;
    return true;
  }
  return false;
}

bool o1() {
  if (match("I")) {
    if (match("I")) {
      match("I");
    }
    return true;
  }
  return true;
}

bool m1() {
  if (match("IX"))
    return true;
  if (match("IV"))
    return true;

  if (match("V")) {
    return o1();
  }

  return o1();
}

bool o2() {
  if (match("X")) {
    if (match("X")) {
      match("X");
    }
    return true;
  }
  return true;
}

bool m2() {
  if (match("XC"))
    return true;
  if (match("XL"))
    return true;

  if (match("L")) {
    return o2();
  }

  return o2();
}

bool l2() { return m2() && m1(); }

bool o3() {
  if (match("C")) {
    if (match("C")) {
      match("C");
    }
    return true;
  }
  return true;
}

bool m3() {
  if (match("CM"))
    return true;
  if (match("CD"))
    return true;

  if (match("D")) {
    return o3();
  }

  return o3();
}

bool l3() { return m3() && l2(); }

bool o4() {
  if (match("M")) {
    if (match("M")) {
      match("M");
    }
    return true;
  }
  return true;
}

bool num() { return o4() && l3(); }

bool is_valid_roman(const char *str) {
  input = str;
  position = 0;

  if (!num() || *input == '\0') {
    return false;
  }

  return input[position] == '\0';
}

int main() {
  const char *test_cases[] = {
      "I",  "II", "III", "IV",  "V",  "VI",  "VII",       "VIII",
      "IX", "X",  "XI",  "XIV", "XV", "XIX", "XX",        "XXX",
      "XL", "L",  "LX",  "XC",  "C",  "CC",  "CCC",       "CD",
      "D",  "DC", "CM",  "M",   "MM", "MMM", "MMMCMXCIX", "IIII",
      "VX", "IC", "XM",  "",    NULL};

  for (int i = 0; test_cases[i] != NULL; i++) {
    printf("'%s': %s\n", test_cases[i],
           is_valid_roman(test_cases[i]) ? "Valid" : "Invalid");
  }

  return 0;
}

