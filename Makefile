# ------------------------------------------
# Red-black tree implementation
# Henrik Peters
# ------------------------------------------

# Binary target name
TARGET ?= rbtree

# Compiler configuration
CC = g++
CPPFLAGS = -c -std=c++14 -Wall -Wextra
LDFLAGS = 

# Source code
SOURCE=$(wildcard *.cpp)
HEADER=$(wildcard *.h)
OBJECTS=$(SOURCE:.cpp=.o)

# Targets
.PHONY: all clean help rebuild
default: all
all: $(TARGET)

# Compiling source code files
%.o : %.cpp $(HEADER)
	@echo "Compiling $@"
	$(CC) $(CPPFLAGS) -o $@ $<
	@echo "Compiling done"

# Linking to executable
$(TARGET): $(OBJECTS)
	@echo "Linking $@"
	$(CC) $(LDFLAGS) $(OBJECTS) -o $(TARGET)
	@echo "Linking done"

# Remove created objects
clean:
	rm -f $(OBJECTS) $(TARGET)
	@echo "cleanup done"

rebuild: clean all

# Show help
help:
	@echo "Options:"
	@echo "make all      - create program"
	@echo "make rebuild  - clean up and create program"
	@echo "make clean    - clean up"
	@echo "make help     - show this help text"
