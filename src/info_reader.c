#include "info_reader.h"

#define MAX_PROCS 128

typedef struct {
  int processor;
  char *vendor_id;
  int family;
  int core_id;
  int physical_id;
  char *model_name;
} Cpu;

int int_allocator(char *s) {
  int value = atoi(s);
  return value;
}

char *string_allocator(char *s) {
  char *value = malloc(strlen(s) + 1);
  strcpy(value, s);
  return value;
}

void set_processor(Cpu *cpu, int data) { cpu->processor = (int)data; }

void set_vendor_id(Cpu *cpu, char *data) { cpu->vendor_id = data; }

void set_family(Cpu *cpu, int data) { cpu->family = (int)data; }

void set_core_id(Cpu *cpu, int data) { cpu->core_id = (int)data; }

void set_physical_id(Cpu *cpu, int data) { cpu->physical_id = (int)data; }

void set_model_name(Cpu *cpu, char *data) { cpu->model_name = data; }

int main(int argc, char const *argv[]) {
  Cpu cpus[MAX_PROCS];

  KeyFinder keys[] = {
      {"processor", (CopyAllocator *)int_allocator, (Setter *)set_processor},
      {"vendor_id", (CopyAllocator *)string_allocator, (Setter *)set_vendor_id},
      {"cpu family", (CopyAllocator *)int_allocator, (Setter *)set_family},
      {"core id", (CopyAllocator *)int_allocator, (Setter *)set_core_id},
      {"physical id", (CopyAllocator *)int_allocator,
       (Setter *)set_physical_id},
      {"model name", (CopyAllocator *)string_allocator,
       (Setter *)set_model_name}};

  Parser parser = {.storage = (void **)cpus,
                   .capacity = MAX_PROCS,
                   .storage_struct_size = sizeof(Cpu),
                   .keys = keys,
                   .nb_keys = 6,
                   .file = fopen("/proc/cpuinfo", "r")};

  parse(&parser);

  for (unsigned int i = 0; i < parser.nb_stored; ++i) {
    printf("========== PROC[%d] ==========\n", i);
    printf("Processor: %d\n", cpus[i].processor);
    printf("Vendor ID: %s\n", cpus[i].vendor_id);
    printf("Family: %d\n", cpus[i].family);
    printf("Core ID: %d\n", cpus[i].core_id);
    printf("Physical ID: %d\n", cpus[i].physical_id);
    printf("Model Name: %s\n", cpus[i].model_name);
    printf("==============================\n");
    free(cpus[i].vendor_id);
    free(cpus[i].model_name);
  }
  return 0;
}

