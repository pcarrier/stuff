CFLAGS=-Wall -Wextra -Werror -pedantic
CC=gcc

all: rotten ok

rotten: repro.c
	${CC} ${CFLAGS} -DROTTEN -o $@ $^

ok: repro.c
	gcc -Wall -Wextra -pedantic -o $@ $^

clean:
	rm -f rotten ok
