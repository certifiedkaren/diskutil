#define _XOPEN_SOURCE 500

#include <ftw.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define ARR_SIZE 250

struct file_info {
  const char *fpath;
  off_t fsize;
};

struct file_info file_arr[ARR_SIZE];
int file_idx = 0;

int comp(const void *a, const void *b);
off_t fsize(const char *file);

int visit(const char *fpath, const struct stat *sb, int tflag,
          struct FTW *ftwbuf) {
  if (file_idx >= ARR_SIZE)
    return 0;
  file_arr[file_idx].fpath = strdup(fpath);
  off_t file_size = fsize(fpath);
  file_arr[file_idx].fsize = file_size;
  file_idx++;
  return 0;
}

int main(void) {
  if (nftw(".", visit, 10, 0) == -1) {
    perror("nftw() error");
    return 1;
  };
  int file_arr_len = sizeof(file_arr) / sizeof(file_arr[0]);

  qsort(file_arr, file_idx, sizeof(file_arr[0]), comp);
  for (int i = 0; i < file_idx; i++) {
    printf("file path: %s\n", file_arr[i].fpath);
    printf("file size (bytes): %ld\n", file_arr[i].fsize);
  }
}

off_t fsize(const char *file) {
  struct stat st;

  if (stat(file, &st) == 0)
    return st.st_size;

  return -1;
}

int comp(const void *a, const void *b) {
  const struct file_info *file_a = a;
  const struct file_info *file_b = b;
  if (file_a->fsize > file_b->fsize)
    return -1;
  if (file_a->fsize < file_b->fsize)
    return 1;
  return 0;
}
