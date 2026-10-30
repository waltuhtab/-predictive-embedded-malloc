# Compiler settings
CC = gcc
CFLAGS = -O3 -I.

# Directories
BIN_DIR = bin
EXP_DIR = exp

# Core library source
CORE_SRC = adaptive_mem.c

# Define the final executable targets
TARGETS = $(BIN_DIR)/run_exp1 $(BIN_DIR)/run_exp2

# Default rule when you just type 'make'
all: $(BIN_DIR) $(TARGETS)

# Rule to create the bin directory if it doesn't exist
$(BIN_DIR):
	mkdir $(BIN_DIR)

# Rule to build Experiment 1
$(BIN_DIR)/run_exp1: $(CORE_SRC) $(EXP_DIR)/exp1_fragmentation.c
	$(CC) $(CFLAGS) $^ -o $@

# Rule to build Experiment 2
$(BIN_DIR)/run_exp2: $(CORE_SRC) $(EXP_DIR)/exp2_zerocopy.c
	$(CC) $(CFLAGS) $^ -o $@

# Rule to clean up built files ('make clean')
clean:
	rmdir /S /Q $(BIN_DIR)

.PHONY: all clean