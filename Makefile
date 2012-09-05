all:
	./waf build

indent:
	indent -kr -nut */*.c */*/*.c

clean:
	rm *~ */*~ */*/*~

.PHONY: all indent clean
