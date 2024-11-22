SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj

CC = cc

CFLAGS = -Wall -Wextra -O2 -std=c99

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

TOTAL := $(shell echo $(SRCS) | wc -w)

TARGET = fuse
TARGET_NAME = Fuse

COLOR_RESET=\033[0m
COLOR_GREEN=\033[32m
COLOR_RED=\033[31m
COLOR_YELLOW=\033[33m

compile: message check_cc $(TARGET)

message:
	@echo "😄  Hello, $(USER). Welcome to $(TARGET_NAME) build system."
	@echo "🚀  Compilation Started..."

$(TARGET): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	@echo -e "[$(TOTAL)/$(TOTAL)] $(COLOR_GREEN)Linking C executable $(BUILD_DIR)/$(TARGET)$(COLOR_RESET)"
	@$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(OBJS)
	@echo -e "[$(TOTAL)/$(TOTAL)] Built target $(TARGET)"
	

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@CURRENT=$$(expr $(shell echo $(OBJS) | tr ' ' '\n' | grep -n "$@" | cut -d: -f1) + 0); \
	echo -e "[$$CURRENT/$(TOTAL)] $(COLOR_GREEN)Building C object $@$(COLOR_RESET)"; \
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@echo -e "🧹  $(COLOR_YELLOW)Cleaning build directory$(COLOR_RESET)"
	@rm -rf $(BUILD_DIR)

configure:
	./x.sh

check_cc:
	@if command -v cc >/dev/null 2>&1; then \
		COMPILER=$$(cc --version | head -n 1); \
		echo -e "$(COLOR_GREEN)✅  C compiler found: $$COMPILER$(COLOR_RESET)"; \
	else \
		echo -e "$(COLOR_RED)❌  C compiler not found. Please install it.$(COLOR_RESET)"; \
		exit 1; \
	fi

.PHONY: all clean message configure

install: compile
	./build/fuse