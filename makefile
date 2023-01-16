.PHONY: all clean mojitos mojitos_group debug format

SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

OBJECTS = $(addprefix $(OBJ_DIR)/, mojitos.o counters.o rapl.o frapl.o network.o load.o infiniband.o temperature.o)
OBJECTS_GRP = $(subst _individual,_group, $(OBJECTS))

CC = gcc
CFLAGS = -std=gnu99 -O3 -Wall -Wextra -Werror -Wpedantic

ASTYLE = astyle --style=kr -xf -s4 -k3 -n -Z -Q


# depending on the context it may need to be changed to all: mojitos mojitos_group
all: mojitos

mojitos: $(OBJ_DIR) $(BIN_DIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN_DIR)/mojitos $(OBJECTS) -lpowercap

$(OBJ_DIR)/counters_%.o: $(SRC_DIR)/counters_%.c $(SRC_DIR)/counters.h $(SRC_DIR)/counters_option.h
	$(CC) $(CFLAGS) -c $< -o $@

$(SRC_DIR)/counters_option.h: $(SRC_DIR)/counters_option.py
	python3 ./$(SRC_DIR)/counters_option.py > $(SRC_DIR)/counters_option.h

$(OBJ_DIR)/mojitos.o: $(SRC_DIR)/mojitos.c $(SRC_DIR)/counters_option.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.c $(SRC_DIR)/%.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

debug: CFLAGS += -DDEBUG -g
debug: all

format:
	$(ASTYLE) $(SRC_DIR)/*.c $(SRC_DIR)/*.h

clean:
	\rm -f $(OBJ_DIR)/* $(BIN_DIR)/*
	\rm -f $(SRC_DIR)/counters_option.h
