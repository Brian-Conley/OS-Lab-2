CC = gcc
CFLAGS = -Wall -Werror

TARGET = smokers

SRC = smokers.c
OBJ = smokers.o

# Build target executable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

# Compile smokers source file
smokers.o: smokers.c
	$(CC) $(CFLAGS) -c $< -o $@

# Run the program
.PHONY: run
run: $(TARGET)
	./$(TARGET)

# Clean build files
.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ)
