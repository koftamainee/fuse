SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

CC = clang

CFLAGS = -O2 -std=c99

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

TOTAL := $(shell echo $(SRCS) | wc -w)

TARGET = fuse
TARGET_NAME = Fuse

all: message compile

message:
	@echo "😄  Hello, $(USER). Welcome to $(TARGET_NAME) build system."
	@echo "🚀  Compilation Started..."

compile: $(OBJS)
	@mkdir -p $(BUILD_DIR)
	@echo "🔧  Linking $(TARGET_NAME)..."
	$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(OBJS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@CURRENT=$$(expr $(shell echo $(OBJS) | tr ' ' '\n' | grep -n "$@" | cut -d: -f1) + 0); \
	echo "[$$CURRENT/$(TOTAL)] Building C object $@"; \
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

configure:
	@if command -v python3 >/dev/null 2>&1; then \
		./x.py; \
	else \
		./x.sh; \
	fi

.PHONY: all clean message configure

