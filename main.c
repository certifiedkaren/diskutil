#define _XOPEN_SOURCE 500

#include <ftw.h>
#include <limits.h>
#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

off_t fsize(const char *file);

int visit(const char *fpath, const struct stat *sb, int tflag,
          struct FTW *ftwbuf) {
  off_t file_size = fsize(fpath);
  printf("file path %s\n", fpath);
  printf("file size: %ld\n", file_size);
  return 0;
}

int main(void) {
  if (nftw(".", visit, 10, 0) == -1) {
    perror("nftw() error");
    return 1;
  };
}

off_t fsize(const char *file) {
  struct stat st;

  if (stat(file, &st) == 0)
    return st.st_size;

  return -1;
}
