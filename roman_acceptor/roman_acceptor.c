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

bool T() {
  if (match("M")) {
    if (match("M")) {
      match("M");
    }
    return true;
  }
  return false;
}

bool H() {
  if (match("D")) {
    if (match("C")) {
      if (match("C")) {
        match("C");
      }
    }
    return true;
  }

  if (match("C")) {
    if (match("D") || match("M")) {
      return true;
    }
    if (match("C")) {
      match("C");
    }
    return true;
  }

  return false;
}

bool E() {
  if (match("L")) {
    if (match("X")) {
      if (match("X")) {
        match("X");
      }
    }
    return true;
  }

  if (match("X")) {
    if (match("L") || match("C")) {
      return true;
    }
    if (match("X")) {
      match("X");
    }
    return true;
  }

  return false;
}

bool O() {
  if (match("V")) {
    if (match("I")) {
      if (match("I")) {
        match("I");
      }
    }
    return true;
  }

  if (match("I")) {
    if (match("V") || match("X")) {
      return true;
    }
    if (match("I")) {
      match("I");
    }
    return true;
  }

  return false;
}

bool T_opt() {
  return T() || true;
}

bool H_opt() {
  return H() || true;
}

bool E_opt() {
  return E() || true;
}

bool O_opt() {
  return O() || true;
}

bool num() {
  return (T() && H_opt() && E_opt() && O_opt()) || (H() && E_opt() && O_opt()) || (E() && O_opt()) || O();
}

bool is_valid_roman(const char *str) {
  input = str;
  position = 0;

  if (!num()) return false;

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

