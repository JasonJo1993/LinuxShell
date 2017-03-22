

int main() {
  int fd = 7;
  char *buf;
  write(0, "sup", 3);
  read(0, buf, 10);
  printf("%s\n", buf);
}
