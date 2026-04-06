CC := gcc
CFLAGS := -Wall -Wextra

SRC_DIR := src
BUILD_DIR := build

SRC := $(shell find $(SRC_DIR) -name '*.c')
OBJ := $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)

INC_DIRS := $(shell find $(SRC_DIR) -type d)
CFLAGS += $(addprefix -I, $(INC_DIRS))

all: client server

server: $(OBJ)
	$(CC) -D SERVER $(CFLAGS) -c $(SRC_DIR)/main.c -o $(BUILD_DIR)/main.o
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

client: $(OBJ)
	$(CC) $(OBJ) -o $@ $(LDFLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

run-server: server
	./server

run-client: client
	./client

clean:
	rm -rf $(BUILD_DIR) server client
