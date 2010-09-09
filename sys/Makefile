CC = gcc
CFLAGS = -Wextra -Wall -Werror -pedantic
CPORTABLEFLAGS = -std=c99 -D_XOPEN_SOURCE
CLINUXFLAGS =

bins := hugepages_doublecheck.l hugepages_maxalloc.l \
	errnos.p mmap_doublecheck.p

all: $(bins)

%.p: %.c
	$(CC) $(CFLAGS) $(CPORTABLEFLAGS) -o $@ $^

%.l: %.c
	$(CC) $(CFLAGS) $(CLINUXFLAGS)    -o $@ $^
clean:
	rm $(bins)
