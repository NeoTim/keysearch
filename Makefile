keysearch-64: keysearch.c
	gcc -std=gnu99 -Wall -o $@ $<

keysearch-32: keysearch.c
	gcc -Wall -o $@ $< -m32
