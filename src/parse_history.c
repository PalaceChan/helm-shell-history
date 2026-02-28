#define _XOPEN_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define TS_DIGITS 10

static void usage(void) {
  fprintf(
      stderr,
      "Usage: parse_history HISTORY_FILE HIST_TIMEFORMAT CANDIDATE_LIMIT\n");
}

static void die(const char *msg) {
  fprintf(stderr, "Error: %s\n", msg);
  exit(1);
}

static void die_errno(const char *msg) {
  perror(msg);
  exit(1);
}

typedef struct Record {
  size_t start;
  size_t length;
} Record;

static size_t parse_cand_limit(const char *s) {
  if (!s || s[0] == '\0') {
    die("CANDIDATE_LIMIT is required");
  }
  if (s[0] == '-') {
    die("CANDIDATE_LIMIT must be non-negative");
  }

  errno = 0;
  char *end = NULL;
  unsigned long long v = strtoull(s, &end, 10);
  if (errno != 0 || end == s || *end != '\0') {
    die("invalid CANDIDATE_LIMIT");
  }
  if (v > (unsigned long long)(~(size_t)0)) {
    die("CANDIDATE_LIMIT too large");
  }

  return (size_t)v;
}

static int parse_epoch_seconds(const char *p, time_t *out) {
  unsigned long long v = 0;
  for (int i = 0; i < TS_DIGITS; ++i) {
    char c = p[i];
    if (c < '0' || c > '9') {
      return 0;
    }
    v = v * 10ULL + (unsigned long long)(c - '0');
  }

  *out = (time_t)v;
  return 1;
}

static size_t parse_record(Record *r, const char *buf, size_t buf_len, long j) {
  size_t digits = 0;
  r->length = 0;

  while (j >= 0) {
    char c = buf[j];

    if (c >= '0' && c <= '9') {
      digits++;
    } else if (c == '#' && (digits == TS_DIGITS)) {
      int done = 0;

      if (((size_t)j + TS_DIGITS + 1) < buf_len &&
          buf[j + TS_DIGITS + 1] == '\n') {
        if (j == 0) {
          done = 1;
        } else if (buf[j - 1] == '\n') {
          done = 1;

          if (j >= TS_DIGITS + 2) {
            size_t digits2 = 0;
            for (int i = 1; i <= TS_DIGITS; i++) {
              if (buf[j - 1 - i] >= '0' && buf[j - 1 - i] <= '9') {
                digits2++;
              }
            }
            if (digits2 == TS_DIGITS && buf[j - TS_DIGITS - 2] == '#') {
              done = 0;
            }
          }
        }
      }

      if (done) {
        if (r->length < TS_DIGITS + 2) {
          return 0;
        }
        r->start = (size_t)j;
        return r->length;
      }
    } else {
      digits = 0;
    }

    if (j == 0) {
      break;
    }

    j--;
    r->length++;
  }

  return 0;
}

static void dump_records(const Record *records, const char *buf, size_t buf_len,
                         size_t nr, const char *time_fmt) {
  char time_buf[256];

  for (size_t i = 0; i < nr; ++i) {
    const Record *r = &records[i];

    if (r->start + TS_DIGITS + 1 >= buf_len) {
      die("record out of bounds");
    }

    time_t t;
    if (!parse_epoch_seconds(&buf[r->start + 1], &t)) {
      die("failed to parse epoch seconds");
    }

    struct tm tm;
    if (!localtime_r(&t, &tm)) {
      die("localtime_r failed");
    }

    size_t time_len = strftime(time_buf, sizeof(time_buf), time_fmt, &tm);
    time_buf[time_len] = ' ';
    time_len++;

    size_t rec_end = r->start + 1 + r->length;
    if (rec_end > buf_len) {
      die("record out of bounds");
    }

    size_t cmd_start = r->start + TS_DIGITS + 2;
    if (cmd_start > rec_end) {
      die("record is malformed");
    }

    size_t cmd_end = rec_end;
    if (cmd_end > cmd_start && buf[cmd_end - 1] == '\n') {
      cmd_end--;
    }
    size_t cmd_len = cmd_end - cmd_start;

    fprintf(stdout, "%zu ", i);
    fwrite(time_buf, 1, time_len, stdout);
    fwrite(&buf[cmd_start], 1, cmd_len, stdout);
    fputc('\0', stdout);
  }

  if (fflush(stdout) != 0) {
    if (errno == EPIPE) {
      return;
    }
    die_errno("fflush");
  }
}

int main(int argc, char *argv[]) {
  if (argc < 4) {
    usage();
    exit(1);
  }

  const char *hist_file = argv[1];
  const char *time_fmt = argv[2];
  size_t cand_limit = parse_cand_limit(argv[3]);
  if (cand_limit == 0) {
    exit(0);
  }

  int fd = open(hist_file, O_RDONLY);
  if (fd == -1) {
    die_errno("open");
  }

  struct stat sb;
  if (fstat(fd, &sb) == -1) {
    die_errno("fstat");
  }

  if (sb.st_size == 0) {
    close(fd);
    exit(0);
  }

  char *buf = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
  if (buf == MAP_FAILED) {
    die_errno("mmap");
  }
  close(fd);

  if (cand_limit > (~(size_t)0) / sizeof(Record)) {
    die("CANDIDATE_LIMIT too large");
  }

  Record *records = calloc(cand_limit, sizeof(Record));
  if (!records) {
    die_errno("calloc");
  }

  long j = sb.st_size - 1;
  size_t nr = 0;

  while (nr < cand_limit && j >= 0) {
    Record *r = &records[nr];
    size_t len = parse_record(r, buf, sb.st_size, j);
    if (len == 0) {
      if (nr == 0) {
        die("failed to parse history file (expected timestamped history)");
      }
      break;
    }

    if (len > (size_t)j) {
      die("record length exceeds buffer position");
    }

    j -= (long)(len + 1);
    nr++;
  }

  dump_records(records, buf, sb.st_size, nr, time_fmt);

  munmap(buf, sb.st_size);
  free(records);
  exit(0);
}
