CC = gcc
CFLAGS = -Wall -I./src
SRC_DIR = src
TEST_DIR = test
BUILD_DIR = build

# Source files
SRC_FILES = $(wildcard $(SRC_DIR)/*.c)
OBJ_FILES = $(SRC_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Test utils
TEST_UTILS = $(BUILD_DIR)/test_utils.o

# Test files (excluding test_utils.c)
TEST_FILES = $(filter-out $(TEST_DIR)/test_utils.c, $(wildcard $(TEST_DIR)/test_*.c))
TEST_BINS = $(TEST_FILES:$(TEST_DIR)/%.c=$(BUILD_DIR)/%)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST_UTILS): $(TEST_DIR)/test_utils.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/test_%: $(TEST_DIR)/test_%.c $(OBJ_FILES) $(TEST_UTILS)
	$(CC) $(CFLAGS) $^ -o $@ -lm

.PHONY: all clean tests

all: tests

tests: $(TEST_BINS)

clean:
	rm -rf $(BUILD_DIR)