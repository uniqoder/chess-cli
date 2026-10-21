# Compiler and base flags
CXX := gcc
CXXFLAGS := -Wall -Wextra -Iinclude
OPTFLAGS := -O2
DEBUGFLAGS := -g

# Directories
SRC_DIR := src
BUILD_DIR := build
TARGET := chess-cli

# Source discovery
SRCS := $(shell find $(SRC_DIR) -name '*.c')
OBJS := $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

# Default target (normal build)
all: $(BUILD_DIR)/$(TARGET)

# Normal build
$(BUILD_DIR)/$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Release build
release: CXXFLAGS += $(OPTFLAGS)
release: $(BUILD_DIR)/$(TARGET)

# Debug build
debug: CXXFLAGS += $(DEBUGFLAGS)
debug: $(BUILD_DIR)/$(TARGET)

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Utility targets
run: all
	./$(BUILD_DIR)/$(TARGET)

clean:
	rm -rf $(BUILD_DIR)

.PHONY: all release run clean
