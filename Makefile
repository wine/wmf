all:
	mkdir -p bin
	gcc -D _GNU_SOURCE -Wall -O3 -o bin/wmf src/wmf.c

install:
	sudo cp bin/wmf /usr/bin/wmf
