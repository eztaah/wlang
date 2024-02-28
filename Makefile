TARGET = build/prog
SRC_DIR = src
BUILD_DIR = build
SOURCES = $(SRC_DIR)/main.c $(SRC_DIR)/lexer.c
CFLAGS = -Wall -Werror

$(TARGET): $(SOURCES)
	mkdir -p $(BUILD_DIR)
	gcc $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
