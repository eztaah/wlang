TARGET = compiler
SRC_DIR = src
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/lexer.cpp $(SRC_DIR)/parser.cpp $(SRC_DIR)/asm_generator.cpp $(SRC_DIR)/semantic_analysis.cpp
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