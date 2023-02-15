/*******************************************************
 Copyright (C) 2023-2023 Georges Da Costa <georges.da-costa@irit.fr>

    This file is part of Mojitos.

    Mojitos is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Mojitos is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with MojitO/S.  If not, see <https://www.gnu.org/licenses/>.

*******************************************************/


// ~/mojitos/doc/$ gcc -Wall -Wextra -Wpedantic -O3 -o info_reader_ex info_reader_ex.c ./../src/util.c && ./info_reader_ex
#include "./../lib/info_reader.h"

#define MAX_PROCS 64

// -----------------------------The storage
typedef struct {
    unsigned int processor;
    char *vendor_id;
    unsigned int family;
    unsigned int core_id;
    unsigned int physical_id;
    char *model_name;
} Cpu;

// --------------IMPLEMENTING THE INTERFACE

// -- Define the behaviour if the attempted value is an int
GenericPointer int_allocator(char *s)
{
    unsigned int value = atoi(s);
    return (GenericPointer) value;
}

// -- Define the behaviour if the attempted value is a string
GenericPointer string_allocator(char *s)
{
    char *value = malloc(strlen(s) + 1);
    strcpy(value, s);
    return (GenericPointer) value;
}

// -- Define the processor setter
void set_processor(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->processor = data;
}

// -- Define the vendor_id setter
void set_vendor_id(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->vendor_id = (char *) data;
}

// -- Define the family setter
void set_family(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->family = data;
}

// -- Define the core_id setter
void set_core_id(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->core_id = data;
}

// -- Define the physical_id setter
void set_physical_id(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->physical_id = data;
}

// -- Define the model_name setter
void set_model_name(GenericPointer storage, GenericPointer data)
{
    Cpu *cpu = (Cpu *) storage;
    cpu->model_name = (char *) data;
}

int main()
{
    Cpu cpus[MAX_PROCS];

    // -- Define the setter, the allocator for each key / separator.
    KeyFinder keys[] = {
        {.key = "processor", .delimiter = ": ", .copy = (CopyAllocator *) int_allocator, .set = (Setter *)set_processor},
        {.key = "vendor_id", .delimiter = ": ", .copy = (CopyAllocator *) string_allocator, .set = (Setter *)set_vendor_id},
        {.key = "cpu family", .delimiter = ": ", .copy = (CopyAllocator *) int_allocator, .set = (Setter *)set_family},
        {.key = "core id", .delimiter = ": ", .copy = (CopyAllocator *) int_allocator, .set = (Setter *)set_core_id},
        {.key = "physical id", .delimiter = ": ", .copy = (CopyAllocator *) int_allocator, .set = (Setter *)set_physical_id},
        {.key = "model name", .delimiter = ": ", .copy = (CopyAllocator *) string_allocator, .set = (Setter *)set_model_name}
    };

    size_t nb_keys = sizeof(keys)/sizeof(KeyFinder);

    // -- Init the parser
    Parser parser = {
        .storage = (GenericPointer) cpus,
        .capacity = MAX_PROCS,
        .storage_struct_size = sizeof(Cpu),
        .keys = keys,
        .nb_keys = nb_keys,
        .file = fopen("/proc/cpuinfo", "r")
    };

    // -- Parse the file
    parse(&parser);

    // Print and free the results
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

