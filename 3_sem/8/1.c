#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>


static const int error_code = -1;
static const int success_code = 0;
static const int input_file_error = 1;

typedef struct Item {
  int value;
  uint32_t next_pointer;
} item_t;

int main(int args, char *argv[]) {
	int input_file = open(argv[1], O_RDONLY);
	if (input_file == error_code) {
		close(input_file);
		return input_file_error;
	}
	item_t item;
	if (read(input_file, &item, sizeof(item)) <= 0) {
		close(input_file);
		return success_code;
	}
	while (item.next_pointer) {
		printf("%d ", item.value);
		lseek(input_file, item.next_pointer, SEEK_SET);
		read(input_file, &item, sizeof(item));
	}
	printf("%d", item.value);
	close(input_file);
}
