#include "info_reader.h"
#include <stdint.h>

#ifdef DEBUG
#warning "PTR_TO_TPTR hide a cast warning"
#endif

#define MAX_PROCS 2
typedef struct {
    size_t processor;
    char *vendor_id;
    size_t family;
    size_t core_id;
    size_t physical_id;
    char *model_name;
} Cpu;

GenericPointer int_allocator(char *s)
{
    size_t value = atoi(s);
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

struct cpu_sensor_t {
    //TODO: check the reset of the msr registers
#warning "Check the reset of the msr registers"
    size_t cpu_id;
    size_t package_id;
    char *name;

    int *fd;
    uint64_t energy_units;
    uint64_t core_energy;
    uint64_t pkg_energy;
};

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
                     .nb_keys = 6,
                     .file = fopen("/proc/cpuinfo", "r")
                    };

    parse(&parser);

    for (unsigned int i = 0; i < parser.nb_stored; ++i) {
        printf("========== PROC[%d] ==========\n", i);
        printf("Processor: %ld\n", cpus[i].processor);
        printf("Vendor ID: %s\n", cpus[i].vendor_id);
        printf("Family: %ld\n", cpus[i].family);
        printf("Core ID: %ld\n", cpus[i].core_id);
        printf("Physical ID: %ld\n", cpus[i].physical_id);
        printf("Model Name: %s\n", cpus[i].model_name);
        printf("==============================\n");
        free(cpus[i].vendor_id);
        free(cpus[i].model_name);
    }

    printf("size = %ld\n", sizeof (struct cpu_sensor_t));

    return 0;
}

