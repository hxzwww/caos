#include <openssl/sha.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <values.h>

#define HASH_SIZE (512 / CHAR_BIT)

int main() {
  SHA512_CTX context;
  SHA512_Init(&context);

  char input_buffer[HASH_SIZE];
  while (true) {
    size_t bytes_read = read(STDIN_FILENO, input_buffer, HASH_SIZE);
    if (bytes_read == 0) {
      break;
    }

    SHA512_Update(&context, input_buffer, bytes_read);
  }

  unsigned char hash[HASH_SIZE];
  SHA512_Final(hash, &context);

  printf("0x");
  for (size_t i = 0; i < HASH_SIZE; ++i) {
    printf("%02"PRIx32, hash[i] & 0xFFU);
  }
  printf("\n");
}
