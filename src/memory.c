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

#include <stdio.h>
#include <stdlib.h>
#include <sys/sysinfo.h>
#include <string.h>

#include "util.h"

typedef enum {
    TOTALRAM = 0,
    FREERAM,
    SHAREDRAM,
    BUFFERRAM,
    TOTALSWAP,
    FREESWAP,
    PROCS,
    TOTALHIGH,
    FREEHIGH,
    MEM_UNIT,

    MEMORY_COUNT,
} MemoryKind;

static const char *memory_labels[MEMORY_COUNT] = {
    "totalram", "freeram", "sharedram", "bufferram",
    "totalswap", "freeswap",
    "procs",
    "totalhigh", "freehigh", "mem_unit",
};

unsigned int init_memory(char *none1, void **none2)
{
    UNUSED(none1);
    UNUSED(none2);
    struct sysinfo info;
    if (sysinfo(&info) < 0) {
        fprintf(stderr, "Failed to get the memory information");
        return 0;
    }
    return MEMORY_COUNT;
}

unsigned int get_memory(uint64_t *results, void *none)
{
    UNUSED(none);
    struct sysinfo info;
    if (sysinfo(&info) < 0) {
        fprintf(stderr, "Failed to get the memory information");
        exit(99);
    }

    // Can't use memcpy, the size isn't always the same
    results[TOTALRAM]  = info.totalram;
    results[FREERAM]   = info.freeram;
    results[SHAREDRAM] = info.sharedram;
    results[BUFFERRAM] = info.bufferram;
    results[TOTALSWAP] = info.totalswap;
    results[FREESWAP]  = info.freeswap;
    results[PROCS]     = info.procs;
    results[TOTALHIGH] = info.totalhigh;
    results[FREEHIGH]  = info.freehigh;
    results[MEM_UNIT]  = info.mem_unit;

    return MEMORY_COUNT;
}

void label_memory(char **labels, void *none)
{
    UNUSED(none);
    memcpy(labels, memory_labels, sizeof(char *) * MEMORY_COUNT);
}

void clean_memory(void *none)
{
    UNUSED(none);
    return;
}

