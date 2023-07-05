#include <stdio.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <string.h>

int main(int argc, char **argv) {
  char *password = argv[1];

  unsigned char buffer[1024 * 1024];
  int read_;
  int bytes_read = 0;
  while ((read_ = read(0, buffer + bytes_read, sizeof(buffer) - bytes_read)) > 0) {
    bytes_read += read_;
  }

  unsigned char *salt = buffer + 8;
  unsigned char key[32] = {0};
  unsigned char iv[16] = {0};

  EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();

  EVP_BytesToKey(
          EVP_aes_256_cbc(),
          EVP_sha256(),
          salt,
          password, strlen(password),
          1,
          key,
          iv
  );

  EVP_DecryptInit(
          ctx,
          EVP_aes_256_cbc(),
          key,
          iv
  );

  int buffer_size = bytes_read - 16;
  unsigned char *result = malloc(buffer_size);
  int p_len = buffer_size, f_len = 0;

  EVP_DecryptInit_ex(ctx, NULL, NULL, NULL, NULL);
  EVP_DecryptUpdate(ctx, result, &p_len, buffer + 16, buffer_size);
  EVP_DecryptFinal_ex(ctx, result + p_len, &f_len);

  buffer_size = p_len + f_len;

  for (int i = 0; i < buffer_size; ++i) {
    printf("%c", result[i]);
  }

  EVP_CIPHER_CTX_free(ctx);

  return 0;
}
