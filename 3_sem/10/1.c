#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

struct Item {
  int value;
  uint32_t next_pointer;
};

typedef struct Item Item;

int main(int argc, char* argv[]) {
	int file_fd = open(argv[1], O_RDONLY);
	struct stat file_attributes;
	fstat(file_fd, &file_attributes);
	Item* items = mmap(NULL, file_attributes.st_size, PROT_READ, MAP_PRIVATE, file_fd, 0);
	int item_size = sizeof(Item);
	if (file_attributes.st_size >= item_size) {
		Item* items_new = items;
		while (1) {
			printf("%d\n", le32toh(items_new->value));
			items_new = items + items_new->next_pointer / item_size;
			if (items_new == items) {
				break;
			}
		}
	}
	munmap(items, file_attributes.st_size);
	close(file_fd);
	return 0;
}
