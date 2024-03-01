TARGET = build/prog
SRC_DIR = src
BUILD_DIR = build
SOURCES = $(SRC_DIR)/main.cpp $(SRC_DIR)/lexer.cpp
CFLAGS = -Wall -Werror

$(TARGET): $(SOURCES)
	mkdir -p $(BUILD_DIR)
	g++ -g $(CFLAGS) $(SOURCES) -o $(TARGET)

clean:
	rm -rf $(BUILD_DIR)
