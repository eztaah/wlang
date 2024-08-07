SOURCE_FILES = src/*.c src/lib/*.c src/compiler/*.c

INCLUDE_FLAGS = -Isrc -Isrc/lib -Isrc/compiler
CFLAGS = -Wall -Wextra -std=c2x
DEBUG_FLAG = -g
RELEASE_FLAG = -03

debug:
	gcc $(SOURCE_FILES) $(INCLUDE_FLAGS) $(CFLAGS) $(DEBUG_FLAG) -o wcompiler

release:
	gcc $(SOURCE_FILES) $(INCLUDE_FLAGS) $(CFLAGS) $(RELEASE_FLAG) -o wcompiler

clean:
	rm -f wcompiler
	rm -f out/*