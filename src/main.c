#include <stdio.h>
#include <stdlib.h>

#include "../include/cstring.h"

int main(int argc, char* argv[]) {
  String str = string_from("Hello world");
  if (argc == 1) {
    string_print(str);
    printf("\nHello from %s!\n", argv[0]);
  } else {
    printf("Somthing wrong :(\n");
  }
  string_free(str);
  return EXIT_SUCCESS;
}