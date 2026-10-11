# Executable name
EXEC_NAME = minishell

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g -I$(INC_DIR) -MMD -MP

LDLIBS =
LDFLAGS = 

# Directories
SRC_DIR = ./src
INC_DIR = ./inc
BIN_DIR = ./bin
BUILD_DIR = ./build

# Source and object files
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Executable path
EXEC = $(BIN_DIR)/$(EXEC_NAME)

# Default rule
all: $(EXEC)

# Executable compilation
$(EXEC): $(OBJS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)
#@echo "$(CC) $(LDFLAGS) $(OBJS) -o $@ $(LDLIBS)"

# Object files compilation
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Create directories if necessary
$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Clean
clean:
	rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.d

aclean: clean
	rm -f $(EXEC)

# Force rebuild
rebuild: aclean all

# Include auto-generated dependencies
-include $(DEPS)

# Declaration of "virtual" rules
.PHONY: all clean aclean rebuild