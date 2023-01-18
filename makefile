.POSIX:

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

BIN = mojitos

OBJ =  \
	$(OBJ_DIR)/counters.o \
	$(OBJ_DIR)/rapl.o \
	$(OBJ_DIR)/network.o \
	$(OBJ_DIR)/load.o \
	$(OBJ_DIR)/infiniband.o \
	$(OBJ_DIR)/temperature.o

CC = gcc
CPPFLAGS = -std=gnu99 -Wall -Wextra -Wpedantic -Wno-unused-function
CFLAGS = $(CPPFLAGS) -O3 -Werror
LDFLAGS = -lpowercap

ASTYLE = astyle --style=kr -xf -s4 -k3 -n -Z -Q


all: $(BIN)

$(BIN): $(BIN_DIR) $(OBJ) $(OBJ_DIR)/$(BIN).o
	$(CC) $(LDFLAGS) -o $(BIN_DIR)/$(BIN) $(OBJ) $(OBJ_DIR)/$(BIN).o

$(OBJ): $(OBJ_DIR)
$(OBJ_DIR)/counters.o: $(SRC_DIR)/counters_option.h

$(OBJ_DIR)/$(BIN).o: $(SRC_DIR)/$(BIN).c $(SRC_DIR)/counters_option.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/counters_option.h: $(SRC_DIR)/counters_option.sh
	sh ./$(SRC_DIR)/counters_option.sh > $(SRC_DIR)/counters_option.h

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

debug: CFLAGS = $(CPPFLAGS) -DDEBUG -g -Og
debug: all

format:
	$(ASTYLE) $(SRC_DIR)/*.[ch]

clean:
	\rm -f $(OBJ_DIR)/* $(BIN_DIR)/*
	\rm -f $(SRC_DIR)/counters_option.h

.PHONY: all clean mojitos debug format
