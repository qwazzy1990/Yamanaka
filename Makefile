CC = gcc -lm
CFLAGS = -std=c11 -g

all: ladder

BIN:
	if [ ! -d "bin" ]; then mkdir bin; fi;

INCLUDE:
	if [ ! -d "include" ]; then mkdir include; fi;

SRC: 
	if [ ! -d "src" ]; then mkdir src; fi;

OBJ:
	if [ ! -d "obj" ]; then mkdir obj; fi;

TEST:
	if [ ! -d "test" ]; then mkdir test; fi;

LIB:
	if [ ! -d "lib" ]; then mkdir lib; fi;

ladder: makeladder runladder

makeladder: BIN SRC OBJ INCLUDE TEST
	$(CC) $(CFLAGS) -Iinclude enum.c -o bin/ladder

runladder:
	./bin/ladder -p 5 6 4 3 2 1
clean:
	if [ -d "bin" ]; then rm -rf ./bin/; fi;
