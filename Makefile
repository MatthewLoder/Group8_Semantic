# Compiler
CC = gcc

# Include directories
INCLUDES = -Iinclude

# Source files (now includes parser.c)
SRC = src/lexer/lexer.c src/parser/parser.c src/semantic/semantic.c

# Object files
OBJ = $(SRC:.c=.o)

# Output executable
TARGET = semantic_main

# Compilation flags
CFLAGS = -Wall -Wextra -g $(INCLUDES)

# Default target
all: $(TARGET)

# Link object files into the final executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compile .c to .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
run: $(TARGET)
	./$(TARGET)

# Clean up build files
clean:
	rm -f $(OBJ) $(TARGET)

# Rebuild from scratch
rebuild: clean all
