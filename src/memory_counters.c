#include "memory_option.h"
#include "util.h"
#include <fcntl.h>
#include <info_reader.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *path = "/proc/meminfo";

typedef struct {
    KeyFinder *keys;
    unsigned int count;
    FILE *file;
} MemoryCounters;

GenericPointer long_allocator(char *s)
{
    long value = atol(s);
    return (GenericPointer)value;
}

KeyFinder *build_keyfinder(unsigned int count, unsigned int *indexes)
{
    KeyFinder *keys = (KeyFinder *)calloc(count, sizeof(KeyFinder));
    for (unsigned int i = 0; i < count; i++) {
        unsigned int idx = indexes[i];
        KeyFinder key = {.key = memory_counters[idx],
                         .delimiter = ":",
                         .copy = long_allocator,
                         .set = setter_functions[i]
                        };
        memcpy(&keys[i], &key, sizeof(KeyFinder));
    }
    return keys;
}

void memory_list(char *memory_string, unsigned int *count,
                 unsigned int *indexes)
{
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

unsigned int init_memory_counters(char *args, void **ptr)
{
    unsigned int indexes[NB_COUNTERS];
    unsigned int count = 0;
    memory_list(args, &count, indexes);

    KeyFinder *keys = build_keyfinder(count, indexes);
    FILE *file = fopen(path, "r");

    MemoryCounters *counters = calloc(1, sizeof(MemoryCounters));
    counters->keys = keys;
    counters->count = count;
    counters->file = file;

    *ptr = (void *)counters;
    return count;
}

unsigned int get_memory_counters(uint64_t *results, void *ptr)
{
    MemoryCounters *counters = (MemoryCounters *)ptr;
    fseek(counters->file, 0, SEEK_SET);
    Parser parser = {.storage = (GenericPointer)results,
                     .capacity = 1,
                     .nb_stored = 0,
                     .storage_struct_size = sizeof(uint64_t) * counters->count,
                     .keys = counters->keys,
                     .nb_keys = counters->count,
                     .file = counters->file
                    };

    parse(&parser);
    return counters->count;
}

void label_memory_counters(char **labels, void *ptr)
{
    MemoryCounters *counters = (MemoryCounters *)ptr;
    for (unsigned int i = 0; i < counters->count; i++) {
        labels[i] = counters->keys[i].key;
    }
}

void clean_memory_counters(void *ptr)
{
    MemoryCounters *counters = (MemoryCounters *)ptr;
    fclose(counters->file);
    free(counters->keys);
    free(ptr);
}

void *show_all_memory_counters(void *none1, size_t none2)
{
    for (unsigned int i = 0; i < NB_COUNTERS; i++) {
        printf("%s\n", memory_counters[i]);
    }

    UNUSED(none1);
    UNUSED(none2);
    exit(EXIT_SUCCESS);
    return NULL; /* not reached */
}
