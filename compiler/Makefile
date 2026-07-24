SOURCE_FILES = src/*.c src/lib/*.c src/node/*.c src/token/*.c

INCLUDE_FLAGS = -Isrc -Isrc/lib -Isrc/node -Isrc/token
CFLAGS = -Wall -Wextra -std=c2x
DEBUG_FLAG = -g
RELEASE_FLAG = -03

debug:
	gcc $(SOURCE_FILES) $(INCLUDE_FLAGS) $(CFLAGS) $(DEBUG_FLAG) -o wlangc

release:
	gcc $(SOURCE_FILES) $(INCLUDE_FLAGS) $(CFLAGS) $(RELEASE_FLAG) -o wlangc

clean:
	rm -f wlangc