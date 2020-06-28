src = $(wildcard *.c)
obj = $(src:.c=.o)

CC = gcc
#CFLAGS = -Wall -Wextra -ggdb
CFLAGS = -Wall -Wextra -fomit-frame-pointer -march=native -O3

all: parse_history

parse_history: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)

test: t0 t1 t2

# regular file
t0: parse_history
	test/run.sh ./parse_history $@ '%Y%m%d %T' 9999

# adversarial timestamp
t1: parse_history
	test/run.sh ./parse_history $@ '%Y%m%d %T' 9999

# long command and multiline command
t2: parse_history
	test/run.sh ./parse_history $@ '%Y%m%d %T' 9999

.PHONY: clean
clean:
	rm -f $(obj) parse_history test/output_*.txt
