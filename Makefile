keysearch-64: keysearch.c
	gcc -DARCH64 -Wall -o $@ $<

keysearch-32: keysearch.c
	gcc -Wall -m32 -o $@ $<
