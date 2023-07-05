#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main() {
  char buffer[1024];

  scanf("%s", buffer);

  strupr(buffer);
}