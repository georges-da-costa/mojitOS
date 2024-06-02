/*******************************************************
 Copyright (C) 2022-2023 Georges Da Costa <georges.da-costa@irit.fr>

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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>
#include <errno.h>
#include "util.h"

#define MAX_HEADER 128
#define BUFFER_SIZE 1024

char *get_rapl_string(const char *filename)
{
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        return NULL;
    }

    char *result = malloc(MAX_HEADER);
    int nb = read(fd, result, MAX_HEADER);
    close(fd);
    result[nb - 1] = 0;
    return result;
}

void append(char *name, int i, size_t buffer_size)
{
    size_t name_len = strlen(name);
    char *ptr = name + name_len;

    size_t remaining_space = buffer_size - name_len;
    snprintf(ptr, remaining_space, "%d", i);
}


struct IntelRapl {
    unsigned int nb;
    char **names;
    int *fids;
    uint64_t *values;
    uint64_t *tmp_values;
    uint64_t *modulo;

};
typedef struct IntelRapl IntelRapl;


void add_rapl_source(IntelRapl *rapl, char *name, uint64_t modulo, char *energy_uj)
{
    rapl->nb += 1;
    rapl->names = realloc(rapl->names, sizeof(char **)*rapl->nb);
    rapl->fids = realloc(rapl->fids, sizeof(int *)*rapl->nb);
    rapl->modulo = realloc(rapl->modulo, sizeof(uint64_t)*rapl->nb);
    
    rapl->names[rapl->nb - 1] = malloc(strlen(name) + 1);
    strcpy(rapl->names[rapl->nb - 1], name);
    //printf("%s\n", energy_uj);

    rapl->modulo[rapl->nb - 1] = modulo;
    
    int fd = open(energy_uj, O_RDONLY);

    if (fd < 0) {
        fprintf(stderr, "%s ", energy_uj);
        perror("open");
        exit(1);
    }

    rapl->fids[rapl->nb - 1] = fd;
}


void _get_rapl(uint64_t *values, IntelRapl *rapl)
{
    static char buffer[512];

    for (unsigned int i = 0; i < rapl->nb; i++) {

        if (pread(rapl->fids[i], buffer, 100, 0) < 0) {
            perror("pread");
            exit(1);
        }

        values[i] = strtoull(buffer, NULL, 10);
    }
}

int add_rapl_source_from_str(IntelRapl *rapl, const char*name_base, const int i) {

  char buffer[BUFFER_SIZE];
  
  snprintf(buffer, BUFFER_SIZE, name_base, i);
  strcat(buffer, "name");
  char *tmp = get_rapl_string(buffer);
  if (tmp == NULL)
    return 0;
  append(tmp, i, MAX_HEADER); // tmp contains the name with its index. ex: dram0

  snprintf(buffer, BUFFER_SIZE, name_base, i);
  strcat(buffer, "name");
  uint64_t modulo = strtoull(get_rapl_string(buffer), NULL, 10);

  snprintf(buffer, BUFFER_SIZE, name_base, i);
  strcat(buffer, "energy_uj");
  
  add_rapl_source(rapl, tmp, modulo, buffer);
  free(tmp);
  return 1;
}

unsigned int init_rapl(char *none, void **ptr)
{
    UNUSED(none);
    IntelRapl *rapl = malloc(sizeof(IntelRapl));
    rapl->nb = 0;
    rapl->names = NULL;
    rapl->fids = NULL;
    rapl->modulo = NULL;
    
    char buffer[BUFFER_SIZE];
    char *name_base = "/sys/devices/virtual/powercap/intel-rapl/intel-rapl:%d/";
    char *name_sub = "/sys/devices/virtual/powercap/intel-rapl/intel-rapl:%d/intel-rapl:%d:%s/";

    for (unsigned int i = 0;; i++) {


      if(!add_rapl_source_from_str(rapl, name_base, i))
	break;

      for (unsigned int j = 0;; j++) {
	snprintf(buffer, BUFFER_SIZE, name_sub, i, i, "%d");

	if(!add_rapl_source_from_str(rapl, buffer, j))
	  break;

      }
    }

    rapl->values = calloc(sizeof(uint64_t), rapl->nb);
    rapl->tmp_values = calloc(sizeof(uint64_t), rapl->nb);

    _get_rapl(rapl->values, rapl);

    *ptr = (void *)rapl;
    return rapl->nb;
}


unsigned int get_rapl(uint64_t *results, void *ptr)
{
    IntelRapl *state = (IntelRapl *) ptr;
    _get_rapl(state->tmp_values, state);

    fprintf(stderr, "RAPL\t");
    for (unsigned int i = 0; i < state->nb; i++) {
      fprintf(stderr, "%" PRIu64 "\t", state->values[i]);
    }
    fprintf(stderr, "\n");

    
    for (unsigned int i = 0; i < state->nb; i++) {
        results[i] = modulo_substraction(state->tmp_values[i], state->values[i]);
    }

    memcpy(state->values, state->tmp_values, sizeof(uint64_t)*state->nb);
    return state->nb;
}

void clean_rapl(void *ptr)
{
    IntelRapl *rapl = (IntelRapl *) ptr;

    for (unsigned int i = 0; i < rapl->nb; i++) {
        free(rapl->names[i]);
        close(rapl->fids[i]);
    }

    free(rapl->names);
    free(rapl->fids);
    free(rapl->values);
    free(rapl->tmp_values);
    free(rapl);
}


void label_rapl(char **labels, void *ptr)
{
    IntelRapl *rapl = (IntelRapl *) ptr;

    for (unsigned int i = 0; i < rapl->nb; i++) {
        labels[i] = rapl->names[i];
    }
}
