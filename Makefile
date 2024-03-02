TARGET = prog
SRC_DIR = src
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/lexer.cpp $(SRC_DIR)/parser.cpp $(SRC_DIR)/asm_generator.cpp
CFLAGS = -Wall -Wextra

$(TARGET): $(SOURCES)
	g++ -g $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
