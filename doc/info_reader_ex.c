#include "info_reader.h"
#include <stdint.h>

#define MAX_PROCS 2
#define NB_KEYS 6
typedef struct {
    unsigned int processor;
    char *vendor_id;
    unsigned int family;
    unsigned int core_id;
    unsigned int physical_id;
    char *model_name;
} Cpu;

GenericPointer int_allocator(char *s)
{
    unsigned int value = atoi(s);
    return (GenericPointer) value;
}

GenericPointer string_allocator(char *s)
{
    char *value = malloc(strlen(s) + 1);
    strcpy(value, s);
    return (GenericPointer) value;
}

void set_processor(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->processor = data;
}
void set_vendor_id(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->vendor_id = (char *) data;
}
void set_family(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->family = data;
}
void set_core_id(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->core_id = data;
}
void set_physical_id(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->physical_id = data;
}
void set_model_name(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->model_name = (char *) data;
}

int main(int argc, char const *argv[])
{
    Cpu cpus[MAX_PROCS];
    KeyFinder keys[] = {
        {"processor", ": ", (CopyAllocator *) int_allocator, (Setter *)set_processor},
        {"vendor_id", ": ", (CopyAllocator *) string_allocator, (Setter *)set_vendor_id},
        {"cpu family", ": ", (CopyAllocator *) int_allocator, (Setter *)set_family},
        {"core id", ": ", (CopyAllocator *) int_allocator, (Setter *)set_core_id},
        {"physical id", ": ", (CopyAllocator *) int_allocator,(Setter *)set_physical_id},
        {
            "model name", ": ", (CopyAllocator *) string_allocator,
            (Setter *)set_model_name
        }
    };

    Parser parser = {.storage = (GenericPointer) cpus,
                     .capacity = MAX_PROCS,
                     .storage_struct_size = sizeof(Cpu),
                     .keys = keys,
                     .nb_keys = NB_KEYS,
                     .file = fopen("/proc/cpuinfo", "r")
                    };

    parse(&parser);

    for (unsigned int i = 0; i < parser.nb_stored; ++i) {
        printf("========== PROC[%d] ==========\n", i);
        printf("Processor: %u\n", cpus[i].processor);
        printf("Vendor ID: %s\n", cpus[i].vendor_id);
        printf("Family: %u\n", cpus[i].family);
        printf("Core ID: %u\n", cpus[i].core_id);
        printf("Physical ID: %u\n", cpus[i].physical_id);
        printf("Model Name: %s\n", cpus[i].model_name);
        free(cpus[i].vendor_id);
        free(cpus[i].model_name);
    }
        printf("==============================\n");
    return 0;
}

