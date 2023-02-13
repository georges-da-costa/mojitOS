.POSIX:

SRC_DIR = src
DOC_DIR = doc
OBJ_DIR = obj
BIN_DIR = bin
TESTS_DIR = tests

BIN = mojitos

CAPTOR_OBJ =

include ./sensors.mk

OBJ =  \
	$(CAPTOR_OBJ) \
	$(OBJ_DIR)/util.o

CC = gcc
CPPFLAGS = -std=gnu99 -Wall -Wextra -Wpedantic -Wno-unused-function -I./lib
CFLAGS = $(CPPFLAGS) -O3 -Werror
LDFLAGS =

ASTYLE = astyle --style=kr -xf -s4 -k3 -n -Z -Q


all: $(BIN) man

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
debug: $(BIN)

tests:
	$(CC) $(CPPFLAGS) $(TESTS_DIR)/main.c $(SRC_DIR)/util.c -o $(TESTS_DIR)/run
	$(TESTS_DIR)/run

format:
	$(ASTYLE) $(SRC_DIR)/*.[ch] \
		$(DOC_DIR)/*.[ch] \
		$(TESTS_DIR)/*.[ch]

clean:
	\rm -f $(OBJ_DIR)/* $(BIN_DIR)/* \
		$(SRC_DIR)/counters_option.h \
		$(TESTS_DIR)/run

readme: $(BIN)
	sh ./tools/update-readme-usage.sh

man: $(BIN)
	awk -v "usage=$$($(BIN_DIR)/$(BIN) --dump-opts)" \
		'/^USAGE/ { $$0=usage } 1' \
		doc/$(BIN).pre.1 > doc/$(BIN).1 2>/dev/null

.PHONY: all clean mojitos debug format tests readme man
