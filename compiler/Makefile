SOURCE_FILES = src/*.c src/lib/*.c src/node/*.c src/token/*.c

INCLUDE_FLAGS = -Isrc -Isrc/lib -Isrc/node -Isrc/token
CFLAGS = -Wall -Wextra -std=c2x
DEBUG_FLAG = -g
RELEASE_FLAG = -03

debug:
	gcc $(SOURCE_FILES) $(INCLUDE_FLAGS) $(CFLAGS) $(DEBUG_FLAG) -o wcompiler

release:
	gcc $(SOURCE_FILES) $(INCLUDE_FLAGS) $(CFLAGS) $(RELEASE_FLAG) -o wcompiler

clean:
	rm -f wcompiler
	find ./test -maxdepth 1 ! -name 'test.w' ! -name '.' ! -name '..' -type f -exec rm -f {} +