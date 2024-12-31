CC 			= gcc
CFLAGS 		= -Wall -fPIC -Iinclude
LDFLAGS		= -shared
TARGET_LIB	= lib/libimage.so
TARGET_BIN	= bin/program

SRC_DIR		= src
INCLUDE_DIR = include

SOURCES		= $(wildcard $(SRC_DIR)/*.c)
OBJECTS 	= $(SOURCES:$(SRC_DIR)/%.c=$(SRC_DIR)/%.o)

PROGRAM_SRC = main.c 

all: $(TARGET_LIB) $(TARGET_BIN)

# Compile shared library
$(TARGET_LIB): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^

# Compile object files for the library
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile and link main program
$(TARGET_BIN): $(PROGRAM_SRC) $(TARGET_LIB)
	$(CC) $(CFLAGS) -o $@ $(PROGRAM_SRC) $(TARGET_LIB) -ljpeg

clean:
	rm -f $(SRC_DIR)/*.o $(TARGET_LIB) $(TARGET_BIN)