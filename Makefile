SRC_DIR = src
BUILD_DIR = build
INCLUDE_DIR = include
OBJ_DIR = $(BUILD_DIR)/obj
CONFIG_FILE = install_config.ini
CERT_FILE = certificate.txt
ANALYZER = fuse-analyzer
ANALYZER_RS = fuse-analyzer-rs


CC = cc

CFLAGS = -Wall -Wextra -O2 -std=c99 -g -fsanitize=address -fsanitize=leak -fno-omit-frame-pointer
# all errors, extra errors, 02 optimization level, memory acess sanitaixer, memory leaks sanitizer, save frame pointers

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
compile: message_hello check_config message_start_compilation check_cc $(TARGET_PATH) docs analyzer

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

analyzer:
	@if [ "$(USE_RUST)" = "true" ]; then \
		echo -e "🦀  $(COLOR_YELLOW)Using rust for analyzer, starting compilation. $(COLOR_RESET)"; \
		cd fuse-analyzer-rs && cargo build --release; \
	else \
		echo -e "⚙️  $(COLOR_YELLOW)Using .sh for analyzer, compilation skipped. $(COLOR_RESET)"; \
	fi

# Docs and License
user_man: $(BUILD_DIR)/docs/user_man.pdf
in_instruct: $(BUILD_DIR)/docs/in_instruct.pdf
an_instruct: $(BUILD_DIR)/docs/an_instruct.pdf
license: $(BUILD_DIR)/docs/LICENSE.pdf

$(BUILD_DIR)/docs/user_man.pdf: docs/user_man.tex | $(BUILD_DIR)/docs check_pdflatex
	@echo -e "[1/4]$(COLOR_GREEN) Compiling user_man.tex$(COLOR_RESET)"
	@pdflatex -output-directory=$(BUILD_DIR)/docs docs/user_man.tex >> /dev/null
	@rm -f $(BUILD_DIR)/docs/user_man.aux $(BUILD_DIR)/docs/user_man.log $(BUILD_DIR)/docs/user_man.out indent.log
	@cp $(BUILD_DIR)/docs/user_man.pdf $(DOCS_PATH)/user_man.pdf

$(BUILD_DIR)/docs/in_instruct.pdf: docs/in_instruct.tex | $(BUILD_DIR)/docs check_pdflatex
	@echo -e "[2/4]$(COLOR_GREEN) Compiling in_instruct.tex$(COLOR_RESET)"
	@pdflatex -output-directory=$(BUILD_DIR)/docs docs/in_instruct.tex >> /dev/null
	@rm -f $(BUILD_DIR)/docs/in_instruct.aux $(BUILD_DIR)/docs/in_instruct.log $(BUILD_DIR)/docs/in_instruct.out indent.log
	@cp $(BUILD_DIR)/docs/in_instruct.pdf $(DOCS_PATH)/in_instruct.pdf

$(BUILD_DIR)/docs/an_instruct.pdf: docs/an_instruct.tex | $(BUILD_DIR)/docs check_pdflatex
	@echo -e "[3/4]$(COLOR_GREEN) Compiling an_instruct.tex$(COLOR_RESET)"
	@pdflatex -output-directory=$(BUILD_DIR)/docs docs/an_instruct.tex >> /dev/null
	@rm -f $(BUILD_DIR)/docs/an_instruct.aux $(BUILD_DIR)/docs/an_instruct.log $(BUILD_DIR)/docs/an_instruct.out indent.log
	@cp $(BUILD_DIR)/docs/an_instruct.pdf $(DOCS_PATH)/an_instruct.pdf

$(BUILD_DIR)/docs/LICENSE.pdf: docs/LICENSE.tex | $(BUILD_DIR)/docs check_pdflatex
	@echo -e "[4/4]$(COLOR_GREEN) Compiling LICENSE.tex$(COLOR_RESET)"
	@pdflatex -output-directory=$(BUILD_DIR)/docs docs/LICENSE.tex >> /dev/null
	@rm -f $(BUILD_DIR)/docs/LICENSE.aux $(BUILD_DIR)/docs/LICENSE.log $(BUILD_DIR)/docs/LICENSE.out indent.log
	@cp $(BUILD_DIR)/docs/LICENSE.pdf $(DOCS_PATH)/LICENSE.pdf

$(BUILD_DIR)/docs:
	@mkdir -p $(BUILD_DIR)/docs
docs: user_man in_instruct an_instruct license

$(BUILD_DIR)/presentation: 
		@mkdir -p $(BUILD_DIR)/presentation

presentation: $(BUILD_DIR)/presentation
	@pdflatex -output-directory=$(BUILD_DIR)/presentation presentation/presentation.tex 

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
else
	@echo -e "⚠️  $(COLOR_YELLOW)Running as root. Potentially unsafe.$(COLOR_RESET)"
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
	@awk '/^Certificate of Free Use$$/ {found[1]=1} \
		      /^User: .+$$/ {found[2]=1} \
		      /^E-mail: [a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$$/ {found[3]=1} \
		      /^Phone: \+[0-9]{8,15}$$/ {found[4]=1} \
		      /^License: MIT License$$/ {found[5]=1} \
		      /^Install Date: [0-9]{4}-[0-9]{2}-[0-9]{2}$$/ {found[6]=1} \
		      END { \
		        if (length(found) == 6) { \
		          print "✅  $(COLOR_GREEN)Certificate is valid.$(COLOR_RESET)"; \
		        } else { \
		          print "🔏  $(COLOR_RED)Certificate is invalid. Missing fields:$(COLOR_RESET)"; \
		          if (!found[1]) print "- Certificate of Free Use"; \
		          if (!found[2]) print "- User field"; \
		          if (!found[3]) print "- Valid email"; \
		          if (!found[4]) print "- Valid phone"; \
		          if (!found[5]) print "- License"; \
		          if (!found[6]) print "- Install Date"; \
		          exit 1; \
		        } \
		      }' $(CERT_FILE)

	@echo -e "🔑  Change certificate owner to root"
	@sudo cp $(CERT_FILE) $(CERT_PATH)/$(CERT_FILE)
	@sudo chown root $(CERT_PATH)/$(CERT_FILE)
	@sudo chmod 0644 $(CERT_PATH)/$(CERT_FILE)

	@sudo rm -rf /usr/bin/$(TARGET)
	@echo -e "🏄  Installing $(TARGET_NAME) binary"
	@sudo cp $(BUILD_DIR)/$(TARGET) $(BIN_PATH)

	@echo -e "🔗  Creating symlinks"
ifneq ($(BIN_PATH),/usr/bin)
	@sudo ln -sf $(BIN_PATH)/$(TARGET) /usr/bin/$(TARGET)
endif

	@sudo ln -sf /etc/$(TARGET) $(BIN_PATH)/etc
	@sudo ln -sf $(SAVE_PATH) $(BIN_PATH)/saves
	@sudo ln -sf $(TEMP_PATH) $(BIN_PATH)/tmp
	@sudo ln -sf $(DOCS_PATH) $(BIN_PATH)/docs

	@echo -e "📊  Installing fuse-analyzer"
	@if [ "$(USE_RUST)" = "true" ]; then \
		sudo mkdir -p /etc/$(TARGET)/$(ANALYZER); \
		sudo cp $(ANALYZER_RS)/target/release/fuse-analyzer-rs /etc/$(TARGET)/$(ANALYZER)/$(ANALYZER_RS); \
		sudo cp -r $(ANALYZER)/awk/ /etc/$(TARGET)/$(ANALYZER)/awk; \
		sudo ln -sf /etc/$(TARGET)/$(ANALYZER)/$(ANALYZER_RS) /usr/bin/$(ANALYZER); \
	else \
		sudo cp -r $(ANALYZER) /etc/$(TARGET)/$(ANALYZER); \
		sudo ln -sf /etc/$(TARGET)/$(ANALYZER)/$(ANALYZER).sh /usr/bin/$(ANALYZER); \
	fi

	@echo -e "🤖  Creating uninstalling scripts"
	@sudo cp scripts/uninstall.sh /etc/$(TARGET)/uninstall.sh
	@sudo cp scripts/remove.sh /etc/$(TARGET)/remove.sh
	@sudo cp scripts/remove_all.sh /etc/$(TARGET)/remove_all.sh

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
	@rm -rf $(CERT_FILE)

clean_compile: clean_tmp clean_docs
	@echo -e "🧹  $(COLOR_YELLOW)Cleaning fuse binary$(COLOR_RESET)"
	@rm -rf $(BUILD_DIR)/$(TARGET)
	@rm -rf $(ANALYZER_RS)/target

clean_docs:
	@echo -e "🧹  $(COLOR_YELLOW)Cleaning compiled docs$(COLOR_RESET)"
	@rm -rf $(BUILD_DIR)/docs

clean: clean_tmp clean_certificate clean_config clean_docs

clean_all: clean clean_compile

uninstall:
	@if [ ! -f "/etc/$(TARGET)/uninstall.sh" ]; then \
	echo -e "🚫  $(COLOR_RED)$(TARGET_NAME) is not installed.$(COLOR_RESET)"; \
	else \
	echo -e "🧹  $(COLOR_YELLOW)Uninstalling $(TARGET_NAME)$(COLOR_RESET)"; \
	sudo sh "/etc/$(TARGET)/uninstall.sh"; \
	fi

remove:
	@if [ ! -f "/etc/$(TARGET)/remove.sh" ]; then \
    echo -e "🚫  $(COLOR_RED)$(TARGET_NAME) is not installed.$(COLOR_RESET)"; \
    else \
    echo -e "🧹  $(COLOR_YELLOW)Uninstalling $(TARGET_NAME) and deleting logs$(COLOR_RESET)"; \
    sudo sh /etc/$(TARGET)/remove.sh; \
    fi

remove_all:
	@if [ ! -f "/etc/$(TARGET)/remove_all.sh" ]; then \
    echo -e "🚫  $(COLOR_RED)$(TARGET_NAME) is not installed.$(COLOR_RESET)"; \
    else \
    echo -e "🧹  $(COLOR_YELLOW)Uninstalling $(TARGET_NAME), deleting logs and saves$(COLOR_RESET)"; \
    sudo sh /etc/$(TARGET)/remove_all.sh; \
    fi


.PHONY: all clean configure check_config install message_hello message_start_compilation presentation

BIN_PATH  = $(shell awk -F '=' '/bin_path/{print $$2}' install_config.ini)
CERT_PATH = $(shell awk -F '=' '/cert_path/{print $$2}' install_config.ini)
TEMP_PATH = $(shell awk -F '=' '/temp_path/{print $$2}' install_config.ini)
SAVE_PATH = $(shell awk -F '=' '/save_path/{print $$2}' install_config.ini)
DOCS_PATH = $(shell awk -F '=' '/docs_path/{print $$2}' install_config.ini)
USE_RUST  = $(shell awk -F '=' '/use_rust/{print $$2}' install_config.ini)
