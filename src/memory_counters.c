#include "meminfo_option.h"
#include <info_reader.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *path = "/proc/meminfo";

GenericPointer long_allocator(char *s) {
  long value = atol(s);
  return (GenericPointer)value;
}

KeyFinder *build_keyfinder(unsigned int count, unsigned int *indexes) {
  KeyFinder *keys = (KeyFinder *)calloc(count, sizeof(KeyFinder));
  for (unsigned int i = 0; i < count; i++) {
    unsigned int idx = indexes[i];
    KeyFinder key = {.key = memory_counters[idx],
                     .delimiter = ":",
                     .copy = long_allocator,
                     .set = setter_functions[i]};
    memcpy(&keys[i], &key, sizeof(KeyFinder));
  }
  return keys;
}

void memory_list(char *memory_string, unsigned int *count,
                 unsigned int *indexes) {
  char *token;
  *count = 0;

  while ((token = strtok(memory_string, ",")) != NULL) {
    memory_string = NULL;

    unsigned int i;
    for (i = 0; i < NB_COUNTERS; i++) {
      if (strcmp(memory_counters[i], token) == 0) {
        (*count)++;
        indexes[*count - 1] = i;
        break;
      }
    }

    if (i == NB_COUNTERS) {
      fprintf(stderr, "Unknown memory counter: %s\n", token);
      exit(EXIT_FAILURE);
    }

    if ((*count) > NB_COUNTERS) {
      fprintf(stderr, "Too much counters, there are probably duplicates\n");
      exit(EXIT_FAILURE);
    }
  }
}

int main(int argc, char **argv) {

  if (argc != 2) {
    fprintf(stderr, "Usage ... [elem1,elem2...]\n");
    exit(EXIT_FAILURE);
  }

  unsigned int indexes[NB_COUNTERS];
  unsigned int count = 0;
  memory_list(argv[1], &count, indexes);

  printf("%d, count \n", count);

  KeyFinder *keys = build_keyfinder(count, indexes);
  uint64_t value[count];

  // -- Init the parser
  Parser parser = {.storage = (GenericPointer)&value,
                   .capacity = 1,
                   .storage_struct_size = sizeof(uint64_t) * count,
                   .keys = keys,
                   .nb_keys = count,
                   .file = fopen(path, "r")};

  // -- Parse the file
  while (1) {
    parse(&parser);
    for (unsigned int i = 0; i < count; i++) {
      printf("%s: %" PRIu64 "\n", keys[i].key, value[i]);
    }
  }

  free(keys);

  // Print and free the results

  fclose(parser.file);
  return 0;
}
