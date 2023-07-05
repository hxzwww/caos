#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define FUSE_USE_VERSION 30

#include <fuse.h>

////////////////////////////////////////////////////////////////////////////////

typedef struct {
  char *src;
  int help;
} simplefs_options_t;

////////////////////////////////////////////////////////////////////////////////

typedef struct {
  char filename[PATH_MAX];
  size_t filesize;
  char *contents;
} simplefs_file_t;

size_t simplefs_file_count;
simplefs_file_t *simplefs;

#define RAW_DATA_SIZE (1024 * 1024)
char raw_data[RAW_DATA_SIZE];

size_t read_raw(FILE *fs, char *buff, size_t buff_size) {
  size_t total_bytes_read = 0;
  while (total_bytes_read < buff_size) {
    size_t bytes_read = fread(
            buff + total_bytes_read, 1, buff_size - total_bytes_read, fs);
    total_bytes_read += bytes_read;

    if (bytes_read == 0) {
      break;
    }
  }

  return total_bytes_read;
}

void open_filesystem(const char *src) {
  FILE *fs = fopen(src, "r");

  fscanf(fs, "%zu\n", &simplefs_file_count);
  simplefs = calloc(simplefs_file_count, sizeof(simplefs_file_t));
  for (size_t i = 0; i < simplefs_file_count; ++i) {
    fscanf(fs, "%s %zu\n", simplefs[i].filename, &simplefs[i].filesize);
  }

  read_raw(fs, raw_data, RAW_DATA_SIZE);
  char *contents = raw_data;
  for (size_t i = 0; i < simplefs_file_count; ++i) {
    simplefs[i].contents = contents;
    contents += simplefs[i].filesize;
  }

  fclose(fs);
}

////////////////////////////////////////////////////////////////////////////////

int simplefs_readdir(
        const char *path, void *out, fuse_fill_dir_t filler, off_t offset,
        struct fuse_file_info *file_info, enum fuse_readdir_flags flags) {
  if (strcmp(path, "/") != 0) {
    return -ENOENT;
  }

  filler(out, ".", NULL, 0, 0);
  filler(out, "..", NULL, 0, 0);

  for (size_t i = 0; i < simplefs_file_count; ++i) {
    filler(out, simplefs[i].filename, NULL, 0, 0);
  }

  return 0;
}

int simplefs_getattr(
        const char *path, struct stat *out_stat, struct fuse_file_info *file_info) {
  memset(out_stat, 0, sizeof(struct stat));

  if (strcmp(path, "/") == 0) {
    out_stat->st_mode = S_IFDIR | 0755;
    out_stat->st_nlink = 2;
    return 0;
  }

  const char *needle_filename = path + 1;
  for (size_t i = 0; i < simplefs_file_count; ++i) {
    if (strcmp(simplefs[i].filename, needle_filename) == 0) {
      out_stat->st_mode = S_IFREG | 0444;
      out_stat->st_nlink = 1;
      out_stat->st_size = simplefs[i].filesize;
      return 0;
    }
  }

  return -ENOENT;
}

int simplefs_open(const char *path, struct fuse_file_info *file_info) {
  if ((file_info->flags & O_ACCMODE) != O_RDONLY) {
    return -EACCES;
  }

  const char *needle_filename = path + 1;
  for (size_t i = 0; i < simplefs_file_count; ++i) {
    if (strcmp(simplefs[i].filename, needle_filename) == 0) {
      file_info->fh = (uint64_t) (&simplefs[i]);
      return 0;
    }
  }

  return -ENOENT;
}

int simplefs_read(
        const char *path, char *out, size_t size, off_t offset,
        struct fuse_file_info *file_info) {
  simplefs_file_t *file = (simplefs_file_t *) (file_info->fh);

  if (offset >= file->filesize) {
    return 0;
  }
  if (offset + size > file->filesize) {
    size = file->filesize - offset;
  }

  const void *data = file->contents + offset;
  memcpy(out, data, size);

  return size;
}

void simplefs_destroy(void *private_data) {
  free(simplefs);
}

struct fuse_operations operations = {
        .readdir = simplefs_readdir,
        .getattr = simplefs_getattr,
        .open = simplefs_open,
        .read = simplefs_read,
        .destroy = simplefs_destroy
};

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[]) {
  struct fuse_args args = FUSE_ARGS_INIT(argc, argv);

  simplefs_options_t simplefs_options = {};
  struct fuse_opt opt_specs[] = {
          {"--src %s", offsetof(simplefs_options_t, src)},
          {NULL}
  };
  fuse_opt_parse(&args, &simplefs_options, opt_specs, NULL);

  if (simplefs_options.src) {
    open_filesystem(simplefs_options.src);
  }

  int ret_code = fuse_main(args.argc, args.argv, &operations, NULL);
  return ret_code;
}
