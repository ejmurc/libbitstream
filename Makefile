CC := gcc
CFLAGS := -O3 -march=native -flto -Wall -Wextra -Werror -std=c99 -Iinclude -pipe
LDFLAGS := -flto
SRC := src
OBJ := build
OUT := $(OBJ)/main
SRCS := $(wildcard $(SRC)/*.c)
OBJS := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))
HDRS := $(wildcard include/*.h)

.PHONY: all test clean format install check-deps

all: test

test: $(OUT)
	@echo "Running tests..."
	./$(OUT)

$(OUT): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c $(HDRS) | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir -p $(OBJ)

clean:
	rm -rf $(OBJ)

format:
	@command -v clang-format >/dev/null 2>&1 || { echo "Error: clang-format not found but required."; exit 1; }
	clang-format -i $(SRCS) $(HDRS)

install:
	@echo "Installing dependencies..."
	@if command -v apt-get >/dev/null 2>&1; then \
		sudo apt-get update && sudo apt-get install -y clang-format; \
	elif command -v brew >/dev/null 2>&1; then \
		brew install clang-format; \
	elif command -v pacman >/dev/null 2>&1; then \
		sudo pacman -S clang-format; \
	elif command -v dnf >/dev/null 2>&1; then \
		sudo dnf install clang-tools-extra; \
	else \
		echo "Unknown package manager. Please manually install: cmocka, pkg-config, clang-format"; \
		exit 1; \
	fi
	@echo "Dependencies installed successfully."
