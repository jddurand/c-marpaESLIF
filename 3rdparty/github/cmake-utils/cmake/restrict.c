int foo (int *C_RESTRICT ip) {
  return ip[0];
}

int bar (int ip[]) {
  return ip[0];
}

int main() {
  int s[1];
  int *C_RESTRICT t = s;
  t[0] = 0;
  return foo (t) + bar (t);
}
