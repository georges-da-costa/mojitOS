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
#include "./../src/info_reader.h"

#define MAX_PROCS 64
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

int main()
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

