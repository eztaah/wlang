TARGET = compiler
SRC_DIR = src
SOURCES = $(SRC_DIR)/main.cc $(SRC_DIR)/global.cc $(SRC_DIR)/lexer.cc $(SRC_DIR)/parser.cc $(SRC_DIR)/asm_generator.cc
CFLAGS = -Wall -Wextra -std=c++23
DEBUG_FLAGS = -g
RELEASE_FLAGS = -O3
.DEFAULT_GOAL := debug

debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET)

release: CFLAGS += $(RELEASE_FLAGS)
release: $(TARGET)

$(TARGET): $(SOURCES)
	g++ $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm $(TARGET)