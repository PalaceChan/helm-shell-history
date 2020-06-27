src = $(wildcard *.c)
obj = $(src:.c=.o)

CC = gcc
CFLAGS = -Wall -Wextra -ggdb 

parse_history: $(obj)
	$(CC) -o $@ $^ $(CFLAGS)

.PHONY: clean
clean:
	rm -f $(obj) parse_history
