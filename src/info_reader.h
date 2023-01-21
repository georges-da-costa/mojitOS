#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _INFO_READER_H
#define _INFO_READER_H

typedef struct Parser Parser;
typedef struct KeyFinder KeyFinder;

// Transforme the string in the expected format
typedef void *(CopyAllocator)(char *string);
// Set the value into the memory
typedef void(Setter)(void *storage, void *value);

struct KeyFinder {
  char *key;
  // Functions
  CopyAllocator *copy;
  Setter *set;
};

struct Parser {
  void *storage;
  unsigned int nb_stored;
  unsigned int capacity;
  size_t storage_struct_size;

  KeyFinder *keys;
  unsigned int nb_keys;

  FILE *file;
};

char *get_string_value(char *line) {
  char *start = line;
  while (*start != '\0') {
    if (*start == ':' && *(start + 1) == ' ') {
      return (start + 2);
    }
    ++start;
  }
  return NULL;
}

void replace_first(char *string, char find, char replace) {
  char *found = strchr(string, find);
  if (found) {
    *found = replace;
  }
}

unsigned int start_with(const char *prefix, const char *string) {
  size_t prefix_len = strlen(prefix);
  size_t string_len = strlen(string);
  return string_len < prefix_len ? false
                                 : memcmp(prefix, string, prefix_len) == 0;
}

int match(Parser *parser, char *line) {
  for (size_t i = 0; i < parser->nb_keys; i++) {
    KeyFinder *key_finder = &parser->keys[i];

    if (start_with(key_finder->key, line)) {
      char *string_value = get_string_value(line);
      if (get_string_value == NULL) {
        fprintf(stderr, "Error : invalid format for line : %s\n", line);
        exit(EXIT_FAILURE);
      }
      replace_first(string_value, '\n', '\0');
      void *value = key_finder->copy(string_value);

      key_finder->set(
          (parser->storage + (parser->storage_struct_size * parser->nb_stored)),
          value);

      return 1;
    }
  }
  return 0;
}

void parse(Parser *parser) {
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  unsigned int key_assigned = 0;

  while ((read = getline(&line, &len, parser->file)) != -1) {
    if (key_assigned == parser->nb_keys && read > 1) {
      continue;
    }
    if (read == 1) {
      parser->nb_stored++;
      if (parser->nb_stored == parser->capacity) {
        fprintf(stderr, "Error : too much cpus to read.\n");
        exit(EXIT_FAILURE);
      }

      key_assigned = 0;
    }
    key_assigned += match(parser, line);
  }

  if (key_assigned > 0) {
    parser->nb_stored++;
  }

  free(line);
}

#endif
