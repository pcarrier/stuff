CC = gcc
CFLAGS += -Wall -Wextra -pedantic
CPORTABLEFLAGS += -std=c99 -D_XOPEN_SOURCE

bins := \
	fun/nato.portable \
	sys/hugepages_doublecheck.linux \
	sys/hugepages_maxalloc.linux \
	sys/errnos.portable \
	sys/mmap_doublecheck.portable \
	sys/i_segv.portable \
	sys/i_segv2.portable \
	sys/sethostid.linux \
	mini/yes.portable \
	mini/yes2.portable \
	mini/true.portable \
	mini/false.portable \
	mini/echo.portable \
	mini/sync.portable \
	mini/printf.portable \
#	sys/nosymlinks.fuse

all: $(bins)

%.linux: %.c
	$(CC) $(CFLAGS) -o $@ $^

%.portable: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CPORTABLEFLAGS)

%.fuse: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(shell pkg-config --cflags --libs fuse)

indent:
	indent -kr -nut */*.c

clean:
	rm $(bins) *~ */*~
