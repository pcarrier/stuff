CC = gcc
CFLAGS += -Wall -Wextra -pedantic
CPORTABLEFLAGS += -std=c99 -D_XOPEN_SOURCE

bins := \
	fun/nato.portable \
	mini/echo.portable \
	mini/false.portable \
	mini/hostid.portable \
	mini/logname.portable \
	mini/printf.portable \
	mini/sync.portable \
	mini/true.portable \
	mini/yes.portable \
	mini/yes2.portable \
	sys/errnos.portable \
	sys/hugepages_doublecheck.linux \
	sys/hugepages_maxalloc.linux \
	sys/i_segv.portable \
	sys/i_segv2.portable \
	sys/mmap_doublecheck.portable \
	sys/sethostid.linux \
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
