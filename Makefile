SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
OBJ_DIR = $(BUILD_DIR)/obj
CONFIG_FILE = install_config.ini
SERTIFICATE_FILE = certificate.txt

CC = cc

CFLAGS = -Wall -Wextra -O2 -std=c99

SRCS += $(wildcard $(SRC_DIR)/*.c)
SRCS += $(wildcard $(INCLUDE_DIR)/src/*.c)
OBJS := $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(filter $(SRC_DIR)/%,$(SRCS))) \
        $(patsubst $(INCLUDE_DIR)/src/%,$(OBJ_DIR)/%.o,$(filter $(INCLUDE_DIR)/src/%,$(SRCS)))

TOTAL := $(shell echo $(SRCS) | wc -w)

TARGET = fuse
TARGET_NAME = Fuse
TARGET_PATH = $(BUILD_DIR)/$(TARGET)

COLOR_RESET=\033[0m
COLOR_GREEN=\033[32m
COLOR_RED=\033[31m
COLOR_YELLOW=\033[33m

# Binary compilation
compile: message_hello check_config init_config_vars message_start_compilation check_cc $(TARGET_PATH)

message_hello:
ifeq ($(shell id -u),0)
	@echo -e "⚠️  $(COLOR_YELLOW)Running as root. Potentially unsafe.$(COLOR_RESET)"
endif
	@echo "👋  Hello, $(shell whoami). Welcome to $(TARGET_NAME) build system."

message_start_compilation:
	@echo "🚀  Compilation Started..."


$(TARGET_PATH): $(OBJS)
	@mkdir -p $(BUILD_DIR)
	@echo -e "[$(TOTAL)/$(TOTAL)] $(COLOR_GREEN)Linking C executable $(BUILD_DIR)/$(TARGET)$(COLOR_RESET)"
	@$(CC) $(CFLAGS) -o $(BUILD_DIR)/$(TARGET) $(OBJS)
	@echo -e "[$(TOTAL)/$(TOTAL)] Built target $(TARGET)"
	
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@CURRENT=$$(expr $(shell echo $(OBJS) | tr ' ' '\n' | grep -n "$@" | cut -d: -f1) + 0); \
	echo -e "[$$CURRENT/$(TOTAL)] $(COLOR_GREEN)Building C object $@$(COLOR_RESET)"; \
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(INCLUDE_DIR)/src/%
	@mkdir -p $(OBJ_DIR)
	@CURRENT=$$(expr $(shell echo $(OBJS) | tr ' ' '\n' | grep -n "$@" | cut -d: -f1) + 0); \
	echo -e "[$$CURRENT/$(TOTAL)] $(COLOR_GREEN)Building C object $@$(COLOR_RESET)"; \
	$(CC) $(CFLAGS) -I$(INCLUDE_DIR) -c $< -o $@

# Docs and License
shell whoami_man: check_pdflatex
# TODO

in_instruct: check_pdflatex
# TODO

an_instruct: check_pdflatex
# TODO

license:
# TODO

docs: shell whoami_man in_instruct an_instruct

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

check_pdflatex:
	@if command -v pdflatex >/dev/null 2>&1; then \
		COMPILER=$$(pdflatex --version | head -n 1); \
		echo -e "$(COLOR_GREEN)✅  pdfLaTeX compiler found: $$COMPILER$(COLOR_RESET)"; \
	else \
		echo -e "$(COLOR_RED)❌  pdfLaTeX compiler not found. Please install it.$(COLOR_RESET)"; \
		exit 1; \
	fi

check_config:
	@if [ ! -f install_config.ini ] || [ ! -f certificate.txt ]; then \
		echo -e "\n$(COLOR_YELLOW)😅  Seems like you haven't configure project yet.$(COLOR_RESET)"; \
		echo -e "✨  Starting \"x.sh\" script"; \
		echo -e ""; \
		./x.sh; \
	fi

# Installation
install: check_config $(TARGET_PATH) docs
ifneq ($(shell id -u),0)
	@echo -e "✋  $(COLOR_RED)Running \"make install\" require root access$(COLOR_RESET)"
	@echo -e "🔐  $(COLOR_YELLOW)Entering sudo-enabled environment...$(COLOR_RESET)"
endif
	@echo -e "📁  Creating directories"
	@sudo mkdir -p /etc/$(TARGET)
	@sudo cp install_config.ini /etc/fuse/config.ini

	@sudo mkdir -p $(BIN_PATH)
	@sudo mkdir -p /var/log/$(TARGET) && sudo chown $(shell whoami) /var/log/$(TARGET)
	@sudo mkdir -p $(CERT_PATH) && sudo chown $(shell whoami) $(CERT_PATH)
	@sudo mkdir -p $(TEMP_PATH) && sudo chown $(shell whoami) $(TEMP_PATH)
	@sudo mkdir -p $(SAVE_PATH) && sudo chown $(shell whoami) $(SAVE_PATH)
	@sudo mkdir -p $(DOCS_PATH) && sudo chown $(shell whoami) $(DOCS_PATH)

	@echo -e "📝  Checking the certificate for correctness"
# TODO: certificate checking (1 and 2 points)

	@echo -e "🔑  Change certificate owner to root"
# TODO
	@sudo rm -rf /usr/bin/$(TARGET)
	@echo -e "🏄  Installing $(TARGET_NAME) binary"
	@sudo cp $(BUILD_DIR)/$(TARGET) $(BIN_PATH)/$(TARGET)

	@echo -e "🔗  Creating symlinks"
ifneq ($(BIN_PATH),/usr/bin)
	@sudo ln -sf $(BIN_PATH)/$(TARGET) /usr/bin/$(TARGET)
endif
	@sudo ln -sf /etc/$(TARGET) $(BIN_PATH)/etc
	@sudo ln -sf $(SAVE_PATH) $(BIN_PATH)/saves
	@sudo ln -sf $(TEMP_PATH) $(BIN_PATH)/temp
	@sudo ln -sf $(DOCS_PATH) $(BIN_PATH)/docs

	@echo -e "🤖  Creating uninstalling scripts"
	@sudo touch /etc/$(TARGET)/uninstall.sh
	@sudo chmod +x /etc/$(TARGET)/uninstall.sh

	@sudo touch /etc/$(TARGET)/remove.sh
	@sudo chmod +x /etc/$(TARGET)/remove.sh

	@sudo touch /etc/$(TARGET)/remove_all.sh
	@sudo chmod +x /etc/$(TARGET)/remove_all.sh

	@echo -e "\n$(COLOR_GREEN)✨  Installation done! Have a good day  ✨$(COLOR_RESET)"

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
	@echo -e "🧹  $(COLOR_YELLOW)Uninstalling $(TARGET_NAME)$(COLOR_RESET)"
	@sudo /etc/$(TARGET)/uninstall.sh
	
remove:
	@echo -e "🧹  $(COLOR_YELLOW)Uninstalling $(TARGET_NAME) and deleting logs$(COLOR_RESET)"
	@sudo /etc/$(TARGET)/remove.sh

remove_all:
	@echo -e "🧹  $(COLOR_YELLOW)Uninstalling $(TARGET_NAME), deleting logs and saves$(COLOR_RESET)"
	@sudo /etc/$(TARGET)/remove_all.sh

.PHONY: all clean configure check_config init_config_vars install message_hello message_start_compilation

BIN_PATH  = $(shell awk -F '=' '/bin_path/{print $$2}' install_config.ini)
CERT_PATH = $(shell awk -F '=' '/cert_path/{print $$2}' install_config.ini)
TEMP_PATH = $(shell awk -F '=' '/temp_path/{print $$2}' install_config.ini)
SAVE_PATH = $(shell awk -F '=' '/save_path/{print $$2}' install_config.ini)
DOCS_PATH = $(shell awk -F '=' '/docs_path/{print $$2}' install_config.ini)