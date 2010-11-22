CC = gcc
CFLAGS = -Wextra -Wall -pedantic
CPORTABLEFLAGS = -std=c99 -D_XOPEN_SOURCE

bins := hugepages_doublecheck.linux hugepages_maxalloc.linux \
	errnos.portable mmap_doublecheck.portable i_segv.portable i_segv2.portable \
	sethostid.linux

all: $(bins)

%.portable: %.c
	$(CC) $(CFLAGS) $(CPORTABLEFLAGS) -o $@ $^

%.linux: %.c
	$(CC) $(CFLAGS) -o $@ $^
clean:
	rm $(bins)
