/*******************************************************
 Copyright (C) 2019-2023 Georges Da Costa <georges.da-costa@irit.fr>

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

#include <glob.h>

struct Disk {
    uint64_t *values;
    uint64_t *tmp_values;
    int *sources;
    char** labels;
    int nb;
};

#define DISK_VALUES_SIZE 15

void _get_disk(struct Disk* state)
{
    char disk_buffer[1024];

    for (int disk_id = 0; disk_id < state->nb; disk_id++) {
	
	if (pread(state->sources[disk_id], disk_buffer, 1024 - 1, 0) < 0) {
	    perror("pread");
	    exit(1);
	}
	char* ptr = disk_buffer;
	for (int i=0; i < DISK_VALUES_SIZE; i++)
	    ptr = read_int(ptr, &state->values[disk_id*DISK_VALUES_SIZE + i]);
    }
}


// Public interface
unsigned int init_disk(char *argument, void **state_tmp)
{
    UNUSED(argument);

    *state_tmp = malloc(sizeof(struct Disk));
    struct Disk *state = (struct Disk*) *state_tmp;
    state->sources = NULL;
    state->labels = NULL;
    
    glob_t res;
    
    glob("/sys/block/*", 0, NULL, &res);

    if (res.gl_pathc == 0) {
      fprintf(stderr, "No disk found.\n");
      return 0;
    }
    int nb_disk = res.gl_pathc;

    char *_labels_disk[DISK_VALUES_SIZE] = {
	"%s_read_I_Os", "%s_read_merges", "%s_read_sectors", "%s_read_ticks",
	"%s_write_I_Os", "%s_write_merges", "%s_write_sectors", "%s_write_ticks",
	"%s_in_flight", "%s_io_ticks", "%s_time_in_queue", "%s_discard_I_Os",
	"%s_discard_merges", "%s_discard_sectors", "%s_discard_ticks"
    };
    
    char buffer[1024];
    for (int i=0; i<nb_disk; i++) {
	char* disk_path = res.gl_pathv[i];
	char* disk_name = rindex(disk_path, '/')+1;
	    
	state->sources = (int*)realloc(state->sources, sizeof(int)*(i+1));
        sprintf(buffer, "/sys/block/%s/stat", disk_name);
        state->sources[i] = open(buffer, O_RDONLY);

	state->labels = (char**)realloc(state->labels, sizeof(char*)*DISK_VALUES_SIZE*(i+1));
	for (int j=0; j<DISK_VALUES_SIZE; j++) {
	    state->labels[i*DISK_VALUES_SIZE+j] = malloc(1024*sizeof(char));
	    snprintf(state->labels[i*DISK_VALUES_SIZE+j], 1023, _labels_disk[j], disk_name);
	}
    }
    state->nb = nb_disk;

    state->values = (uint64_t*)malloc(sizeof(uint64_t) * state->nb * DISK_VALUES_SIZE);
    state->tmp_values = (uint64_t*)malloc(sizeof(uint64_t) * state->nb * DISK_VALUES_SIZE);

    _get_disk(state);
    memcpy(state->tmp_values, state->values, sizeof(uint64_t)*state->nb*DISK_VALUES_SIZE);

    globfree(&res);

    return state->nb * DISK_VALUES_SIZE;
}

unsigned int get_disk(uint64_t *results, void *state_tmp)
{
    struct Disk *state = (struct Disk*) state_tmp;
    
    _get_disk(state);

    for (int i = 0; i < state->nb * DISK_VALUES_SIZE; i++) {
        results[i] = state->values[i] - state->tmp_values[i];
    }

    memcpy(state->tmp_values, state->values, sizeof(uint64_t)*state->nb*DISK_VALUES_SIZE);
    return state->nb * DISK_VALUES_SIZE;
}

void clean_disk(void *state_tmp)
{
    struct Disk *state = (struct Disk*) state_tmp;

    free(state->values);
    free(state->tmp_values);

    for (int i=0; i<state->nb; i++)
	close(state->sources[i]);
    free (state->sources);
    
    for(int i=0; i<state->nb*DISK_VALUES_SIZE; i++)
	free(state->labels[i]);
    free(state->labels);

    free(state);
}

void label_disk(const char **labels, void *state_tmp)
{
    struct Disk *state = (struct Disk*) state_tmp;

    for (int i = 0; i < state->nb * DISK_VALUES_SIZE; i++) {
        labels[i] = state->labels[i];
    }
}
