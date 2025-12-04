# Makefile for Spell Checker System
# DSA Midterm Project

# Compiler settings
CXX = g++

# Detect OS and set OpenMP flags accordingly
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)
    # macOS - check for Homebrew's libomp
    BREW_PREFIX := $(shell brew --prefix 2>/dev/null || echo "/usr/local")
    LIBOMP_PREFIX := $(shell brew --prefix libomp 2>/dev/null || echo "$(BREW_PREFIX)")
    OMP_FLAGS = -Xpreprocessor -fopenmp -I$(LIBOMP_PREFIX)/include -L$(LIBOMP_PREFIX)/lib -lomp
else
    # Linux
    OMP_FLAGS = -fopenmp
endif

CXXFLAGS = -std=c++17 -Wall -Wextra -O2 $(OMP_FLAGS)
DEBUG_FLAGS = -g -DDEBUG

# Directories
SRC_DIR = src
INC_DIR = include
TEST_DIR = tests
BUILD_DIR = build
BIN_DIR = bin

# Source files
SOURCES = $(SRC_DIR)/trie.cpp \
          $(SRC_DIR)/kdtree.cpp \
          $(SRC_DIR)/astar_spellcheck.cpp \
          $(SRC_DIR)/spellchecker.cpp \
          $(SRC_DIR)/ui.cpp \
          $(SRC_DIR)/benchmark.cpp \
          $(SRC_DIR)/parallel_processor.cpp \
          $(SRC_DIR)/tone_analyzer.cpp \
          $(SRC_DIR)/visualizer.cpp

MAIN_SRC = $(SRC_DIR)/main.cpp
TEST_SRC = $(TEST_DIR)/test_all.cpp

# Object files
OBJECTS = $(SOURCES:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
MAIN_OBJ = $(BUILD_DIR)/main.o
TEST_OBJ = $(BUILD_DIR)/test_all.o

# Executables
TARGET = $(BIN_DIR)/spellchecker
TEST_TARGET = $(BIN_DIR)/test_runner

# Include path
INCLUDES = -I$(INC_DIR)

# Default target
.PHONY: all
all: directories $(TARGET)

# Create necessary directories
.PHONY: directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)

# Main executable
$(TARGET): $(OBJECTS) $(MAIN_OBJ)
	@echo "Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Build complete: $(TARGET)"

# Test executable
$(TEST_TARGET): $(OBJECTS) $(TEST_OBJ)
	@echo "Linking $(TEST_TARGET)..."
	$(CXX) $(CXXFLAGS) $^ -o $@
	@echo "Test build complete: $(TEST_TARGET)"

# Compile source files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@echo "Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile main
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp
	@echo "Compiling main..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Compile tests
$(BUILD_DIR)/test_all.o: $(TEST_DIR)/test_all.cpp
	@echo "Compiling tests..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Build and run tests
.PHONY: test
test: directories $(TEST_TARGET)
	@echo "\nRunning tests..."
	@echo "=================="
	./$(TEST_TARGET)

# Run the main program
.PHONY: run
run: all
	./$(TARGET)

# Run with UI
.PHONY: run-ui
run-ui: all
	./$(TARGET) --ui

# Run benchmarks
.PHONY: benchmark
benchmark: all
	./$(TARGET) --benchmark

# Debug build
.PHONY: debug
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: clean all

# Clean build files
.PHONY: clean
clean:
	@echo "Cleaning build files..."
	rm -rf $(BUILD_DIR)
	rm -rf $(BIN_DIR)
	@echo "Clean complete."

# Clean all including results
.PHONY: cleanall
cleanall: clean
	@echo "Cleaning results..."
	rm -f benchmarks/results/data/*.csv
	rm -f benchmarks/results/data/*.md
	@echo "Full clean complete."

# Install (copy to /usr/local/bin)
.PHONY: install
install: all
	@echo "Installing to /usr/local/bin..."
	cp $(TARGET) /usr/local/bin/spellchecker
	@echo "Installation complete."

# Uninstall
.PHONY: uninstall
uninstall:
	@echo "Removing from /usr/local/bin..."
	rm -f /usr/local/bin/spellchecker
	@echo "Uninstallation complete."

# Help
.PHONY: help
help:
	@echo "Spell Checker System - Makefile Commands"
	@echo "========================================="
	@echo ""
	@echo "Build commands:"
	@echo "  make          - Build the main executable"
	@echo "  make all      - Same as 'make'"
	@echo "  make debug    - Build with debug symbols"
	@echo "  make test     - Build and run unit tests"
	@echo ""
	@echo "Run commands:"
	@echo "  make run      - Run the spell checker"
	@echo "  make run-ui   - Run with interactive UI"
	@echo "  make benchmark- Run performance benchmarks"
	@echo ""
	@echo "Clean commands:"
	@echo "  make clean    - Remove build files"
	@echo "  make cleanall - Remove build files and results"
	@echo ""
	@echo "Install commands:"
	@echo "  make install  - Install to /usr/local/bin"
	@echo "  make uninstall- Remove from /usr/local/bin"
	@echo ""
	@echo "Other:"
	@echo "  make help     - Show this help message"

# Dependencies (auto-generated would be better, but keeping it simple)
$(BUILD_DIR)/trie.o: $(SRC_DIR)/trie.cpp $(INC_DIR)/trie.h
$(BUILD_DIR)/kdtree.o: $(SRC_DIR)/kdtree.cpp $(INC_DIR)/kdtree.h
$(BUILD_DIR)/astar_spellcheck.o: $(SRC_DIR)/astar_spellcheck.cpp $(INC_DIR)/astar_spellcheck.h $(INC_DIR)/trie.h
$(BUILD_DIR)/spellchecker.o: $(SRC_DIR)/spellchecker.cpp $(INC_DIR)/spellchecker.h $(INC_DIR)/trie.h $(INC_DIR)/kdtree.h $(INC_DIR)/astar_spellcheck.h
$(BUILD_DIR)/ui.o: $(SRC_DIR)/ui.cpp $(INC_DIR)/ui.h $(INC_DIR)/spellchecker.h
$(BUILD_DIR)/benchmark.o: $(SRC_DIR)/benchmark.cpp $(INC_DIR)/benchmark.h $(INC_DIR)/spellchecker.h
$(BUILD_DIR)/parallel_processor.o: $(SRC_DIR)/parallel_processor.cpp $(INC_DIR)/parallel_processor.h $(INC_DIR)/spellchecker.h
$(BUILD_DIR)/tone_analyzer.o: $(SRC_DIR)/tone_analyzer.cpp $(INC_DIR)/tone_analyzer.h
$(BUILD_DIR)/visualizer.o: $(SRC_DIR)/visualizer.cpp $(INC_DIR)/visualizer.h
$(BUILD_DIR)/main.o: $(SRC_DIR)/main.cpp $(INC_DIR)/spellchecker.h $(INC_DIR)/ui.h $(INC_DIR)/benchmark.h $(INC_DIR)/parallel_processor.h $(INC_DIR)/tone_analyzer.h $(INC_DIR)/visualizer.h
$(BUILD_DIR)/test_all.o: $(TEST_DIR)/test_all.cpp $(INC_DIR)/trie.h $(INC_DIR)/kdtree.h $(INC_DIR)/astar_spellcheck.h $(INC_DIR)/spellchecker.h
