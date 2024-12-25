# Define variables
CC = gcc
CFLAGS = -Wall -Wextra -Werror
MAIN = main
MAIN_SRC = main.c
CHILD = child_program
CHILD_SRC = child_program.c

# Default target
all: $(MAIN) $(CHILD)

# Rule for compiling main
$(MAIN): $(MAIN_SRC)
	$(CC) $(CFLAGS) -o $(MAIN) $(MAIN_SRC)

# Rule for compiling child
$(CHILD): $(CHILD_SRC)
	$(CC) $(CFLAGS) -o $(CHILD) $(CHILD_SRC)

# Rule for cleaning up
clean:
	rm -f $(MAIN) $(CHILD)

.PHONY: all clean
