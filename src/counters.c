/*******************************************************
 Copyright (C) 2018-2023 Georges Da Costa <georges.da-costa@irit.fr>

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

#include <linux/perf_event.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <asm/unistd.h>
#include <stdint.h>
#include "util.h"


struct Counter {
    int nbcores;
    int nbperf;
    int **counters;
    uint64_t *counters_values;
    uint64_t *tmp_counters_values;

    int *perf_indexes;

};
typedef struct Counter Counter;

#include "counters_option.h"

void *show_all_counters(void *none1, size_t none2)
{
    for (unsigned int i = 0; i < nb_counter_option; i++) {
        printf("%s\n", perf_static_info[i].name);
    }
    UNUSED(none1);
    UNUSED(none2);
    exit(EXIT_SUCCESS);
    return NULL; /* not reached */
}

void perf_type_key(__u32 **perf_type, __u64 **perf_key, int *indexes, int nb)
{
    *perf_type = malloc(nb * sizeof(__u32));
    *perf_key  = malloc(nb * sizeof(__u64));

    for (int i = 0; i < nb; i++) {
        (*perf_key)[i]  = perf_static_info[indexes[i]].perf_key;
        (*perf_type)[i] = perf_static_info[indexes[i]].perf_type;
    }
}
void perf_event_list(char *perf_string, int *nb_perf, int **perf_indexes)
{
    char *token;
    *nb_perf = 0;
    *perf_indexes = NULL;

    while ((token = strtok(perf_string, ",")) != NULL) {
        perf_string = NULL;
        unsigned int i;

        for (i = 0; i < nb_counter_option; i++) {
            if (strcmp(perf_static_info[i].name, token) == 0) {
                (*nb_perf)++;
                (*perf_indexes) = realloc(*perf_indexes, sizeof(int) * (*nb_perf));
                (*perf_indexes)[*nb_perf - 1] = i;
                break;
            }
        }

        if (i == nb_counter_option) {
            fprintf(stderr, "Unknown performance counter: %s\n", token);
            exit(EXIT_FAILURE);
        }
    }
}

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags)
{
    long res = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);

    if (res == -1) {
        perror("perf_event_open");
        fprintf(stderr, "Error opening leader %llx\n", hw_event->config);
        exit(EXIT_FAILURE);
    }

    return res;
}

Counter *_init_counters(const int nb_perf, const __u32 *types, const __u64 *names)
{
    struct perf_event_attr pe;
    unsigned int nbcores = sysconf(_SC_NPROCESSORS_ONLN);
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.size = sizeof(struct perf_event_attr);
    pe.disabled = 1;

    Counter *counters = malloc(sizeof(struct Counter));
    counters->nbperf = nb_perf;
    counters->nbcores = nbcores;
    counters->counters = malloc(nb_perf * sizeof(int *));

    for (int i = 0; i < nb_perf; i++) {
        pe.type = types[i];
        pe.config = names[i];
        counters->counters[i] = malloc(nbcores * sizeof(int));

        for (unsigned int core = 0; core < nbcores; core++) {
            counters->counters[i][core] = perf_event_open(&pe, -1, core, -1, PERF_FLAG_FD_CLOEXEC);
        }
    }

    return counters;
}

void clean_counters(void *ptr)
{
    Counter *counters = (Counter *) ptr;

    for (int counter = 0; counter < counters->nbperf; counter++) {
        for (int core = 0; core < counters->nbcores; core++) {
            close(counters->counters[counter][core]);
        }

        free(counters->counters[counter]);
    }

    free(counters->counters);
    free(counters->counters_values);
    free(counters->tmp_counters_values);
    free(counters->perf_indexes);

    free(counters);
}

void start_counters(Counter *counters)
{
    for (int counter = 0; counter < counters->nbperf; counter++) {
        for (int core = 0; core < counters->nbcores; core++) {
            ioctl(counters->counters[counter][core], PERF_EVENT_IOC_ENABLE, 0);
        }
    }
}

void reset_counters(Counter *counters)
{
    for (int counter = 0; counter < counters->nbperf; counter++) {
        for (int core = 0; core < counters->nbcores; core++) {
            ioctl(counters->counters[counter][core], PERF_EVENT_IOC_RESET, 0);
        }
    }
}

void _get_counters(Counter *counters, uint64_t *values)
{
    for (int i = 0; i < counters->nbperf; i++) {
        uint64_t accu = 0;
        uint64_t count = 0;

        for (int core = 0; core < counters->nbcores; core++) {
            if (-1 == read(counters->counters[i][core], &count, sizeof(uint64_t))) {
                fprintf(stderr, "Cannot read result");
                exit(EXIT_FAILURE);
            }
            accu += count;
        }

        values[i] = accu;
    }
}






unsigned int init_counters(char *args, void **state)
{
    int nb_perf;
    int *perf_indexes = NULL;

    perf_event_list(args, &nb_perf, &perf_indexes);

    __u32 *perf_type;
    __u64 *perf_key;
    perf_type_key(&perf_type, &perf_key, perf_indexes, nb_perf);
    Counter *fd = _init_counters(nb_perf, perf_type, perf_key);
    free(perf_type);
    free(perf_key);

    fd->perf_indexes = perf_indexes;
    fd->counters_values = malloc(nb_perf * sizeof(uint64_t));
    fd->tmp_counters_values = malloc(nb_perf * sizeof(uint64_t));
    start_counters(fd);
    _get_counters(fd, fd->counters_values);
    *state = (void *) fd;

    return nb_perf;
}

unsigned int get_counters(uint64_t *results, void *ptr)
{
    Counter *state = (Counter *) ptr;

    _get_counters(state, state->tmp_counters_values);

    for (int i = 0; i < state->nbperf; i++) {
        results[i] = modulo_substraction(state->tmp_counters_values[i], state->counters_values[i]);
    }

    memcpy(state->counters_values, state->tmp_counters_values, state->nbperf * sizeof(uint64_t));
    return state->nbperf;
}

void label_counters(char **labels, void *ptr)
{
    Counter *state = (Counter *) ptr;

    for (int i = 0; i < state->nbperf; i++) {
        labels[i] = perf_static_info[state->perf_indexes[i]].name;
    }
}
