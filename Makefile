CC = clang
CFLAGS = -Wall -O2 -I./src -Wno-deprecated-declarations -Wno-missing-braces
OBJCFLAGS = -fobjc-arc
LDFLAGS = -framework Metal -framework QuartzCore -framework Cocoa

SRC_DIR = src
BUILD_DIR = build

SOURCES = $(SRC_DIR)/build_macos.m
OBJECTS = $(SOURCES:.c=.o)
OBJECTS := $(OBJECTS:.m=.o)

TARGET = $(BUILD_DIR)/engine

all: $(TARGET)

$(TARGET): $(OBJECTS)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.m
	$(CC) $(CFLAGS) $(OBJCFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(OBJECTS)

.PHONY: all clean
