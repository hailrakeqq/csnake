CC = clang

all:
	rm -rf build
	mkdir -p build 
	$(CC) -g -o build/csnake main.c -lncurses