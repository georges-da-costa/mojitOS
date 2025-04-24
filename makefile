.POSIX:

SRC_DIR = src
DOC_DIR = doc
OBJ_DIR = obj
LIB_DIR = lib
BIN_DIR = bin
TESTS_DIR = tests

BIN = mojitos
BINP = mojitos_prometeus
PREFIX = /usr/local

CC = gcc
CPPFLAGS = -std=gnu99 -Wall -Wextra -Wpedantic -Wno-unused-function -I./lib $(NVML_IFLAGS)
CFLAGS = $(CPPFLAGS) -O3 -Werror
LDFLAGS = $(CAPTOR_LDFLAGS)

ASTYLE = astyle --style=kr -xf -s4 -k3 -n -Z -Q

all: $(BIN) $(BINP) man

CAPTOR_OBJ =
CAPTOR_LDFLAGS =
NVML_IFLAGS =

include ./sensors.mk

OBJ =  \
	$(OBJ_DIR)/util.o \
	$(OBJ_DIR)/info_reader.o \
	$(OBJ_DIR)/libmojitos.o \
	$(OBJ_DIR)/display_manager.o \
	$(CAPTOR_OBJ)
OBJP =  \
	$(OBJ_DIR)/util.o \
	$(OBJ_DIR)/info_reader.o \
	$(OBJ_DIR)/libmojitos.o \
	$(OBJ_DIR)/prometeus_manager.o \
	$(CAPTOR_OBJ)


options:
	@echo BIN: $(BIN)
	@echo BINP: $(BINP)
	@echo CC: $(CC)
	@echo CFLAGS: $(CFLAGS)
	@echo LDFLAGS: $(LDFLAGS)
	@echo OBJ: $(OBJ)

$(BIN): $(BIN_DIR) $(OBJ) $(OBJ_DIR)/$(BIN).o
	$(CC) -o $(BIN_DIR)/$(BIN) $(OBJ) $(OBJ_DIR)/$(BIN).o $(LDFLAGS)

$(BINP): $(BIN_DIR) $(OBJP) $(OBJ_DIR)/$(BIN).o
	$(CC) -o $(BIN_DIR)/$(BIN) $(OBJP) $(OBJ_DIR)/$(BIN).o $(LDFLAGS)  -lmicrohttpd

$(OBJ): $(OBJ_DIR)
$(OBJP): $(OBJ_DIR)
$(OBJ_DIR)/counters.o: $(SRC_DIR)/counters_option.h
$(OBJ_DIR)/memory_counters.o: $(SRC_DIR)/memory_option.h

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/util.o: $(SRC_DIR)/util.c $(SRC_DIR)/util.h
	$(CC) $(CFLAGS) -c $< -o $@


libmojitos: $(BIN_DIR)/libmojitos.a

$(BIN_DIR)/libmojitos.a: $(BIN_DIR) $(OBJ) $(OBJ_DIR)/libmojitos.o
	ar rcs $@ $(OBJ) $(OBJ_DIR)/libmojitos.o

$(SRC_DIR)/counters_option.h: $(SRC_DIR)/counters_option.sh
	sh ./$(SRC_DIR)/counters_option.sh > $(SRC_DIR)/counters_option.h

$(SRC_DIR)/memory_option.h: $(SRC_DIR)/memory_option.sh
	sh ./$(SRC_DIR)/memory_option.sh > $(SRC_DIR)/memory_option.h

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

debug: CFLAGS = $(CPPFLAGS) -DDEBUG -g -Og
debug: $(BIN)

tests: $(OBJ_DIR)/util.o $(OBJ_DIR)/info_reader.o
	$(CC) $(CPPFLAGS) $(OBJ_DIR)/util.o $(OBJ_DIR)/info_reader.o $(TESTS_DIR)/main.c  -o $(TESTS_DIR)/run
	$(TESTS_DIR)/run

format:
	$(ASTYLE) $(SRC_DIR)/*.[ch] \
		$(DOC_DIR)/*.[ch] \
		$(TESTS_DIR)/*.[ch]

clean:
	\rm -f $(OBJ_DIR)/* $(BIN_DIR)/* \
		$(SRC_DIR)/counters_option.h \
		$(SRC_DIR)/memory_option.h \
		$(TESTS_DIR)/run \
		$(DOC_DIR)/test_main_ex \
		$(DOC_DIR)/info_reader_ex

readme:
	sh ./tools/update-readme-usage.sh

man: $(BIN)
	awk -v "usage=$$($(BIN_DIR)/$(BIN) --dump-opts)" \
		'/^USAGE/ { $$0=usage } 1' \
		doc/$(BIN).pre.1 > doc/$(BIN).1 2>/dev/null

install: $(BIN) man
	mkdir -p $(PREFIX)/bin
	cp $(BIN_DIR)/$(BIN) $(PREFIX)/bin/.
	chmod 755 $(PREFIX)/bin/$(BIN)
	mkdir -p $(PREFIX)/share/man/man1
	cp $(DOC_DIR)/$(BIN).1 $(PREFIX)/share/man/man1/.
	chmod 644 $(PREFIX)/share/man/man1/$(BIN).1

uninstall:
	rm -f $(PREFIX)/bin/$(BIN)
	rm -f $(PREFIX)/share/man/man1/$(BIN).1

.PHONY: all clean mojitos debug format tests readme man install uninstall
