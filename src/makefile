src = $(wildcard *.c)
obj = $(src:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -ggdb

all: parse_history

parse_history: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)

test: t0

t0: parse_history
	test/run.sh ./parse_history $@ '%Y%m%d %T' 9999

.PHONY: clean
clean:
	rm -f $(obj) parse_history test/output_*.txt
