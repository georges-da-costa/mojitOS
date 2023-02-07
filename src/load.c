/*******************************************************
 Copyright (C) 2019-2021 Georges Da Costa <georges.da-costa@irit.fr>

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
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "util.h"

#define LOAD_BUFFER_SIZE 1024
#define LOAD_VALUES_SIZE 10
char buffer[LOAD_BUFFER_SIZE];

static int load_fid = -1;
static uint64_t load_values[LOAD_VALUES_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static uint64_t tmp_load_values[LOAD_VALUES_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static char *stat = "/proc/stat";

void _get_load(uint64_t *results)
{
    if (pread(load_fid, buffer, LOAD_BUFFER_SIZE - 1, 0) < 0) {
        perror("pread");
        exit(1);
    }

    int pos = 0;

    while (buffer[pos] > '9' || buffer[pos] < '0') {
        pos++;
    }

    for (int i = 0; i < LOAD_VALUES_SIZE; i++) {
        results[i] = strtoull(buffer + pos, NULL, LOAD_VALUES_SIZE);

        while (buffer[pos] <= '9' && buffer[pos] >= '0') {
            pos++;
        }

        pos++;
    }
}

// Public interface

unsigned int init_load(char *argument, void **state)
{
    UNUSED(argument);
    UNUSED(state);
    load_fid = open(stat, O_RDONLY);
    if (load_fid < 0) {
        fprintf(stderr, "%s ", stat);
        perror("open");
        exit(1);
    }

    _get_load(load_values);
    return LOAD_VALUES_SIZE;
}

unsigned int get_load(uint64_t *results, void *state)
{
    UNUSED(state);
    _get_load(tmp_load_values);

    for (int i = 0; i < LOAD_VALUES_SIZE; i++) {
        results[i] = tmp_load_values[i] - load_values[i];
    }

    memcpy(load_values, tmp_load_values, sizeof(load_values));
    return LOAD_VALUES_SIZE;
}

void clean_load(void *state)
{
    UNUSED(state);
    close(load_fid);
}

char *_labels[LOAD_VALUES_SIZE] = {
    "user", "nice", "system", "idle", "iowait", "irq",
    "softirq", "steal", "guest", "guest_nice"
};
void label_load(char **labels, void *none)
{
    UNUSED(none);

    for (int i = 0; i < LOAD_VALUES_SIZE; i++) {
        labels[i] = _labels[i];
    }
}
