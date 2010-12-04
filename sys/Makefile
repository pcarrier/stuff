CC = gcc
CFLAGS = -Wall -Wextra -pedantic
CPORTABLEFLAGS = -std=c99 -D_XOPEN_SOURCE

bins := hugepages_doublecheck.linux hugepages_maxalloc.linux \
	errnos.portable mmap_doublecheck.portable i_segv.portable i_segv2.portable \
	sethostid.linux nosymlinks.fuse

all: $(bins)

%.linux: %.c
	$(CC) $(CFLAGS) -o $@ $^

%.portable: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CPORTABLEFLAGS)

%.fuse: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(shell pkg-config --cflags --libs fuse)

indent:
	indent -kr *.c

clean:
	rm $(bins) *~
