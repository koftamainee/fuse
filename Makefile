SRC_DIR = src
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
CONFIG_FILE = install_config.ini
SERTIFICATE_FILE = certificate.txt

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

# Binary compilation
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

# Docs and License
user_man:
# TODO

in_instruct:
# TODO

an_instruct:
# TODO

license:
# TODO

docs: user_man in_instrucy an_instruct

# Configuration and checkings
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

# Installation
install: $(BUILD_DIR)/$(TARGET)
	./build/fuse

all: compile docs

# Cleaning
clean_tmp:
	@echo -e "🧹  $(COLOR_YELLOW)Cleaning temporary files$(COLOR_RESET)"
	@rm -rf $(OBJ_DIR)

clean_config:
	@echo -e "🧹  $(COLOR_YELLOW)Cleaning config file$(COLOR_RESET)"
	@rm -rf $(CONFIG_FILE)

clean_certificate:
	@echo -e "🧹  $(COLOR_YELLOW)Cleaning temporary certificate file$(COLOR_RESET)"
	@rm -rf $(SERTIFICATE_FILE)

clean_compile: clean_tmp
	@echo -e "🧹  $(COLOR_YELLOW)Cleaning fuse binary$(COLOR_RESET)"
	@rm -rf $(TARGET)

clean: clean_tmp clean_certificate clean_config 

clean_all: clean clean_compile

uninstall:
# TODO

remove: uninstall
# TODO

remove_all: remove
# TODO

.PHONY: all clean message configure
