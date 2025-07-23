CC := gcc
CFLAGS := -O3 -march=native -flto -Wall -Wextra -Werror -std=c89 -Iinclude -pipe
LDFLAGS := -flto
SRC := src
OBJ := build
OUT := $(OBJ)/main
SRCS := $(wildcard $(SRC)/*.c)
OBJS := $(patsubst $(SRC)/%.c,$(OBJ)/%.o,$(SRCS))

.PHONY: all clean

all: $(OUT)

$(OUT): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $@

$(OBJ)/%.o: $(SRC)/%.c | $(OBJ)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ):
	mkdir -p $(OBJ)

clean:
	rm -rf $(OBJ)
