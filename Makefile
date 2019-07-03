CC = gcc -lm
CFLAGS = -std=c11 -g

all: BIN SRC OBJ INCLUDE TEST

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

ladder: makeladder

makeladder: all
	$(CC) $(CFLAGS) -Iinclude enum.c -o bin/ladder

clean:
	if [ -d "bin" ]; then rm -rf ./bin/; fi;
