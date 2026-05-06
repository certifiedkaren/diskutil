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
#define KiB 1024LL
#define MiB (1024LL * 1024LL)
#define GiB (1024LL * 1024LL * 1024LL)

struct file_info {
  const char *fpath;
  off_t fsize;
};

struct file_info file_arr[ARR_SIZE];
int file_idx = 0;
off_t total_size = 0;

int comp(const void *a, const void *b);
off_t fsize(const char *file);
char *size_to_string(char *buf, size_t buf_size, off_t file_size);
int visit(const char *fpath, const struct stat *sb, int tflag,
          struct FTW *ftwbuf);
void print_help();

int main(int argc, char *argv[]) {
  if (argc > 2) {
    printf("Usage: diskutil [DIRECTORY]\n");
    return 1;
  }

  if (argc == 1) {
    if (nftw(".", visit, 10, 0) == -1) {
      perror("nftw() error");
      return 1;
    };
  }

  if (argc == 2) {
    if (strcmp(argv[1], "help") == 0 || strcmp(argv[1], "--help") == 0 ||
        strcmp(argv[1], "-h") == 0) {
      print_help();
      return 1;
    }

    if (nftw(argv[1], visit, 10, 0) == -1) {
      fprintf(
          stderr,
          "failed to search directory: %s, make sure it's a valid directory\n",
          argv[1]);
      return 1;
    };
  }

  qsort(file_arr, file_idx, sizeof(file_arr[0]), comp);
  for (int i = 0; i < file_idx; i++) {
    char buf[32];
    off_t file_size = file_arr[i].fsize;
    printf("%-8s %s\n", size_to_string(buf, sizeof(buf), file_size),
           file_arr[i].fpath);

    double percentage = ((double)file_size / total_size) * 100.0;
    int pos = percentage / 5;

    char line[21];
    if (pos == 0 && file_size > 0)
      pos++;
    memset(line, '#', pos);
    memset(line + pos, '-', sizeof(line) - pos);
    line[20] = '\0';
    printf("[%s]  %.2f%%\n", line, percentage);
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

char *size_to_string(char *buf, size_t buf_size, off_t file_size) {
  double size = (double)file_size;
  if (size >= GiB) {
    snprintf(buf, buf_size, "%.1fG", size / GiB);
    return buf;
  } else if (size >= MiB) {
    snprintf(buf, buf_size, "%.1fM", size / MiB);
    return buf;
  } else if (size >= KiB) {
    snprintf(buf, buf_size, "%.1fK", size / KiB);
    return buf;
  }
  snprintf(buf, buf_size, "%dB", (int)size);
  return buf;
}

int visit(const char *fpath, const struct stat *sb, int tflag,
          struct FTW *ftwbuf) {
  if (file_idx >= ARR_SIZE)
    return 0;
  file_arr[file_idx].fpath = strdup(fpath);
  off_t file_size = fsize(fpath);
  file_arr[file_idx].fsize = file_size;

  total_size += file_size;
  file_idx++;

  return 0;
}

void print_help() {
  printf("Usage: diskutil [DIRECTORY]\n"
         "\n"
         "Display disk usage for DIRECTORY.\n"
         "If DIRECTORY is omitted, uses current working directory\n"
         "\n"
         "Options:\n"
         "  -h, --help, help     Show this help message\n");
}
