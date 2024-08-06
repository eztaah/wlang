SOURCE_FILES = src/*.c src/lib/*.c
CFLAGS = -Wall -Wextra -std=c2x
DEBUG_FLAG = -g

debug: 
	gcc $(SOURCE_FILES) $(CFLAGS) $(DEBUG_FLAG) -o wcompiler

clean: 
	rm -f wcompiler
	rm -f out/*
