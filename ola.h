

int ola_str_contains(char* str, char c, int n) {
  int x = 0;
  for (int i = 0; i<n; i++) {
    if (str[i] == c) {
      x=i;
      break;
    }
  }
  return x;
}
