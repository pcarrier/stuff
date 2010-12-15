CC = gcc
CFLAGS += -Wall -Wextra -pedantic
CPORTABLEFLAGS += -std=c99 -D_XOPEN_SOURCE
CPTHREADFLAGS += -lpthread

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
	sys/i_segv3.pthread \
	sys/mmap_doublecheck.portable \
	sys/sethostid.linux \
	crap/mmap_and_wait.portable \
	crap/big_swap.portable \
	crap/weird_gcc.portable \
#	sys/nosymlinks.fuse

all: $(bins)

%.linux: %.c
	$(CC) $(CFLAGS) -o $@ $^

%.portable: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CPORTABLEFLAGS)

%.pthread: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CPTHREADFLAGS)

%.fuse: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(shell pkg-config --cflags --libs fuse)

indent:
	indent -kr -nut */*.c

clean:
	rm $(bins) *~ */*~
