src = $(wildcard *.c)
obj = $(src:.c=.o)

CC ?= gcc
#CFLAGS = -Wall -Wextra -ggdb
CFLAGS ?= -Wall -Wextra -fomit-frame-pointer -march=native -O3
LDFLAGS ?=
LDLIBS ?=

all: parse_history

parse_history: $(obj)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

test: t0 t1 t2 t3 t4

# regular file
t0: parse_history
	test/run.sh ./parse_history $@ '%Y%m%d %T' 9999

# adversarial timestamp
t1: parse_history
	test/run.sh ./parse_history $@ '%Y%m%d %T' 9999

# long command and multiline command
t2: parse_history
	test/run.sh ./parse_history $@ '%Y%m%d %T' 9999

# candidate limit of zero
t3: parse_history
	test/run.sh ./parse_history $@ '%Y%m%d %T' 0

# missing trailing newline at EOF
t4: parse_history
	test/run.sh ./parse_history $@ '%Y%m%d %T' 9999

.PHONY: asan
asan: CFLAGS = -Wall -Wextra -O1 -g -fno-omit-frame-pointer -fsanitize=address,undefined
asan: LDFLAGS = -fsanitize=address,undefined
asan: clean parse_history

.PHONY: clean
clean:
	rm -f $(obj) parse_history test/output_*.txt
