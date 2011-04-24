CC = gcc
CFLAGS += -Wall -Wextra -pedantic
CPORTABLEFLAGS += -std=c99 -D_XOPEN_SOURCE -lm
CPTHREADFLAGS += -lpthread

portable := \
	fun/bytes_to_chars.portable \
	fun/mkpasswd.portable \
	fun/nato.portable \
	fun/superglob.portable \
	mini/echo.portable \
	mini/false.portable \
	mini/hostid.portable \
	mini/logname.portable \
	mini/printf.portable \
	mini/sync.portable \
	mini/true.portable \
	mini/yes.portable \
	mini/yes2.portable \
	sys/big_swap.portable \
	sys/i_segv.portable \
	sys/i_segv2.portable \
	sys/mmap_doublecheck.portable \
	crap/mmap_and_wait.portable

others := \
	sys/hugepages_doublecheck.linux \
	sys/hugepages_maxalloc.linux \
	sys/sethostid.linux \
	sys/wtfitf.linux \
	sys/i_segv3.pthread \
#	sys/nosymlinks.fuse

all: $(portable) $(others) sys/errnos
osx: $(portable) sys/errnos

%.linux: %.c
	$(CC) $(CFLAGS) -o $@ $^

%.portable: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CPORTABLEFLAGS)

%.pthread: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(CPTHREADFLAGS)

%.fuse: %.c
	$(CC) $(CFLAGS) -o $@ $^ $(shell pkg-config --cflags --libs fuse)

sys/errnos.h: sys/errnos.list
	bash sys/errnos.h.gen

sys/errnos: sys/errnos.c sys/errnos.h
	$(CC) $(CFLAGS) $(CPORTABLEFLAGS) -o $@ sys/errnos.c

indent:
	indent -kr -nut */*.c

clean:
	rm *~ */*~

purge:
	rm $(portable) $(others) sys/errnos
