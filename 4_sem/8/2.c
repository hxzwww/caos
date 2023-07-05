#include <curl/curl.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
  char *data;
  size_t length;
} buffer_t;

static size_t
callback_function(
        char *ptr, // буфер с прочитанными данными
        size_t chunk_size, // размер фрагмента данных
        size_t nmemb, // количество фрагментов данных
        void *user_data // произвольные данные пользователя
) {
  buffer_t *buffer = user_data;
  size_t total_size = chunk_size * nmemb;

  // в предположении, что достаточно места
  memcpy(buffer->data, ptr, total_size);
  buffer->length += total_size;
  return total_size;
}

int main(int argc, char *argv[]) {
  CURL *curl = curl_easy_init();

  if (curl) {
    CURLcode res;

    // регистрация callback-функции записи
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback_function);

    // указатель &buffer будет передан в callback-функцию
    // параметром void *user_data

    buffer_t buffer;
    buffer.data = calloc(100 * 1024 * 1024, 1);
    buffer.length = 0;
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &buffer);

    curl_easy_setopt(curl, CURLOPT_URL, argv[1]);
    res = curl_easy_perform(curl);

    // дальше можно что-то делать с данными,
    // прочитанными в buffer

    char *title_start = strstr(buffer.data, "<title>");
    char *title_end = strstr(buffer.data, "</title>");

    long len = title_end - title_start;

    int title_shift_len = 7;

    for (int i = 0; i < len - title_shift_len; ++i) {
      printf("%c", title_start[i + title_shift_len]);
    }

    free(buffer.data);
    curl_easy_cleanup(curl);
  }
}

