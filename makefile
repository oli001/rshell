CFLAGS = -ansi -pedantic -Wall -Werror

all:  rshell dir

rshell:
	g++ $(CFLAGS) src/main.cpp -o rshell
dir:
	mkdir bin;
	mv rshell bin

clean:
	rm -rf bin
