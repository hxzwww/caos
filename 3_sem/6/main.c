#include <stdio.h>

extern double avx_sin(double x);

int main() {
  double res = avx_sin(1.57079632679);
  printf("%f", res);
}
