/*******************************************************
 Copyright (C) 2018-2021 Georges Da Costa <georges.da-costa@irit.fr>

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

#include "counters.h"

struct _counter_t
{
    int nbcores;
    int nbperf;
    int *counters;
};

static long perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
                            int cpu, int group_fd, unsigned long flags)
{
    long res = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
    if (res == -1)
        {
            perror("perf_event_open");
            fprintf(stderr, "Error opening leader %llx\n", hw_event->config);
            exit(EXIT_FAILURE);
        }
    return res;
}

counter_t init_counters(const int nb_perf, const __u32 *types, const __u64 *names)
{
    struct perf_event_attr pe;
    struct _counter_t *counters = malloc(sizeof(struct _counter_t));

    unsigned int nbcores = sysconf(_SC_NPROCESSORS_ONLN);
    counters->nbcores = nbcores;
    counters->nbperf = nb_perf;
    memset(&pe, 0, sizeof(struct perf_event_attr));
    pe.size = sizeof(struct perf_event_attr);
    pe.disabled = 1;

    pe.read_format = PERF_FORMAT_GROUP;
    counters->counters = malloc((nbcores + 1) * sizeof(int));

    for (int core = 0; core < nbcores; core++)
        {
            counters->counters[core] = -1;
            for (int idperf = 0; idperf < nb_perf; idperf ++)
                {
                    pe.type = types[idperf];
                    pe.config = names[idperf];
                    int res = perf_event_open(&pe, -1, core, counters->counters[core], PERF_FLAG_FD_CLOEXEC);
                    if (counters->counters[core] == -1)
                        {
                            counters->counters[core] = res;
                        }
                }
        }
    return counters;
}

void clean_counters(counter_t counters)
{
    for (int core = 0; core < counters->nbcores; core++)
        {
            close(counters->counters[core]);
        }
    free(counters->counters);
    free(counters);
}

void start_counters(counter_t counters)
{
    for (int core = 0; core < counters->nbcores; core++)
        {
            ioctl(counters->counters[core], PERF_EVENT_IOC_ENABLE, PERF_IOC_FLAG_GROUP);
        }
}
void reset_counters(counter_t counters)
{
    for (int core = 0; core < counters->nbcores; core++)
        {
            ioctl(counters->counters[core], PERF_EVENT_IOC_RESET, PERF_IOC_FLAG_GROUP);
        }
}

struct read_format
{
    uint64_t nr;
    struct
    {
        uint64_t value;
    } values[];
};

void get_counters(counter_t counters, long long *values)
{
    int nb_perf = counters->nbperf;
    size_t buffer_size = sizeof(uint64_t) * (1 + nb_perf);
    struct read_format *buffer = NULL;
    if (buffer == NULL)
        {
            buffer = malloc(buffer_size);
        }

    memset(values, 0, nb_perf * sizeof(long long));

    for (int core = 0; core < counters->nbcores; core++)
        {
            if (-1 == read(counters->counters[core], buffer, buffer_size))
                {
                    perror("PB Lecture resultat");
                    exit(EXIT_FAILURE);
                }
            for (int idperf = 0; idperf <= nb_perf; idperf++)
                {
                    values[idperf] += buffer->values[idperf].value;
                }
        }
    reset_counters(counters);
}
