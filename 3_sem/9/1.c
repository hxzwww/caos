#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int read_(const char* file, void* ptr, size_t size) {
    ssize_t chars_read = 0;
    while (1) {
    	chars_read = read(open(file, O_RDONLY), ptr, size);
    	if (!chars_read) {
			break;
    	}
        if (chars_read == -1) {
            return 1;
        }
        ptr += chars_read;
        size -= chars_read;
    }
    return 0;
}

int elf(char* line) {
	return (line[0] == 0x7f) && (line[1] == 'E') && (line[2] == 'L') && (line[3] == 'F');
}

int executable(char* line) {
	char endline[] = " \n";
	line[strcspn(line, endline)] = 0;
	return access(line + 2, F_OK | X_OK) != -1;
}

int file_incorrect(const char* file) {
    char line[PATH_MAX];
    if (read_(file, line, PATH_MAX)) {
        return 1;
    }
	if (!(elf(line)) && !(executable(line))) {
		return 1;
	}
    return 0;
}

int main() {
    char file[PATH_MAX];
    char* line_end = NULL;
    struct stat st;
    while (fgets(file, PATH_MAX, stdin)) {
        line_end = memchr(file, '\n', PATH_MAX);
        if (line_end) {
            *line_end = '\0';
        }
        if (lstat(file, &st) == 0 && st.st_mode & S_IXUSR) {
            if (file_incorrect(file)) {
                printf("%s ", file);
            }
        }
    }
    return 0;
}
