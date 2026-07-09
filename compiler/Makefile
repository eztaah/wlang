SOURCE_FILES = src/*.c src/lib/*.c
CFLAGS = -Wall -Wextra -std=c23 
DEBUG_FLAG = -g

debug: 
	gcc $(SOURCE_FILES) $(CFLAGS) $(DEBUG_FLAG) -o compiler

clean: 
	rm -f compiler
