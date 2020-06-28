#define _XOPEN_SOURCE
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define TS_DIGITS 10

void usage() {
    printf("Usage: parse_history HISTORY_FILE HIST_TIMEFORMAT CANDIDATE_LIMIT\n");
}

void die(char* msg) {
    perror(msg);
    exit(1);
}

typedef struct Record
{
    size_t start;
    size_t length;
} Record;

int debug = 0;

size_t parse_record(Record* r, char* buf, size_t j) {
    assert(r->length == 0);

    int done = 0;
    size_t digits = 0;
    size_t start_j = j;
    while (!done) {
	if (buf[j] >= '0' && buf[j] <= '9') {
	    digits++;
	}
	else if (buf[j] == '#' && (digits == TS_DIGITS)) {
	    if (j == 0) {
		done = 1;
	    }
	    else if (j >= TS_DIGITS + 2 && buf[j-1] == '\n') {
		size_t digits2 = 0;
		for (int i = 1; i <= TS_DIGITS; i++) {
		    if (buf[j - 1 - i] >= '0' && buf[j - 1 - i] <= '9') {
			digits2++;
		    }
		}
		if (!(digits2 == TS_DIGITS && buf[j - 1 - TS_DIGITS - 1] == '#')) {
		    done = 1;
		}
	    }
	}
	else {
	    digits = 0;
	}

	if (!done) {
	    j--;
	    r->length++;
	}
    }

    assert(digits == TS_DIGITS);
    assert(r->length >= TS_DIGITS + 2);

    r->start = j;
    r->length = start_j - j - 1; //no newline

    return r->length;
}

void dump_records(Record* records, char* buf, size_t nr, char* time_fmt) {
    struct tm tm;
    memset(&tm, 0, sizeof(struct tm));
    char time_buf[256];

    setbuf(stdout, NULL);
    
    for (size_t i = 0; i < nr; ++i) {
	Record* r = &records[i];

	strptime(&buf[r->start + 1], "%s", &tm);
	size_t time_len = strftime(time_buf, sizeof(time_buf), time_fmt, &tm);
	time_buf[time_len] = ' ';

	printf("%lu ", i);
	write(1, &time_buf, time_len+1);
	write(1, &buf[r->start + TS_DIGITS + 2], r->length - TS_DIGITS - 3);
	write(1, "\0", 1);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
	usage();
	exit(1);
    }

    char* hist_file = argv[1];
    char* time_fmt = argv[2];
    size_t cand_limit = atoi(argv[3]);

    int fd = open(hist_file, O_RDONLY);
    if (fd == -1) {
	die("open");
    }

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
	die("fstat");
    }
    
    char *buf = mmap(NULL, sb.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (buf == MAP_FAILED) {
	die("mmap");
    }

    Record* records = calloc(cand_limit, sizeof(Record));
    if (!records) {
	die("malloc");
    }
    
    size_t nr = 0;
    size_t j = sb.st_size - 1;
    Record* r = &records[nr];

    assert(buf[j] == '\n');
    while (nr < cand_limit && j > 1) {
	size_t len = parse_record(r, buf, j);
	assert(len <= j);
	j -= len;
	
	nr++;
	r = &records[nr];
    }

    dump_records(records, buf, nr, time_fmt);

    munmap(buf, sb.st_size);
    free(records);
    close(fd);
    exit(0);
}
