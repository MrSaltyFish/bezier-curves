CC = gcc
CFLAGS = -Wall -Wextra -ggdb -std=c11 -pedantic `pkg-config --cflags sdl2` -lm
LIBS = `pkg-config --libs sdl2`
BUILD_DIR = build
SRC_DIR = src
TARGET = $(BUILD_DIR)/bezier

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(SRC_DIR)/main.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC_DIR)/main.c $(LIBS)

run: $(TARGET)
	clear
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

