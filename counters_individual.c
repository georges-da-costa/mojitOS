/*******************************************************
 Copyright (C) 2018-2019 Georges Da Costa <georges.da-costa@irit.fr>

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

#include "counters.h"

struct _counter_t {
  int nbcores;
  int nbperf;
  int **counters;
};
 
static long
perf_event_open(struct perf_event_attr *hw_event, pid_t pid,
		int cpu, int group_fd, unsigned long flags) {
  long res = syscall(__NR_perf_event_open, hw_event, pid, cpu, group_fd, flags);
  if (res == -1) {
    perror("perf_event_open");
    fprintf(stderr, "Error opening leader %llx\n", hw_event->config);
    exit(EXIT_FAILURE);
  }
  return res;
}

counter_t init_counters(const int nb_perf, const __u32 *types, const __u64 *names) {
  struct perf_event_attr pe;
  unsigned int nbcores = sysconf(_SC_NPROCESSORS_ONLN);
  memset(&pe, 0, sizeof(struct perf_event_attr));
  pe.size = sizeof(struct perf_event_attr);
  pe.disabled = 1;

  counter_t counters = malloc(sizeof(struct _counter_t));
  counters->nbperf = nb_perf;
  counters->nbcores=nbcores;
  counters->counters=malloc(nb_perf*sizeof(int*));
  for (int i=0; i<nb_perf; i++) {
    pe.type = types[i];
    pe.config = names[i];
    counters->counters[i] = malloc(nbcores*sizeof(int));

    for (int core=0; core<nbcores; core++) {
      counters->counters[i][core] = perf_event_open(&pe, -1, core, -1, PERF_FLAG_FD_CLOEXEC);
    }
  }
  return counters;
}

void clean_counters(counter_t counters) {
  for(int counter=0; counter<counters->nbperf; counter++) {
    for(int core=0; core<counters->nbcores; core++)
      close(counters->counters[counter][core]);
    free(counters->counters[counter]);
  }
  free(counters->counters);
  free(counters);
}

void start_counters(counter_t counters) {
  for(int counter=0; counter<counters->nbperf; counter++)
    for(int core=0; core<counters->nbcores; core++)
      ioctl(counters->counters[counter][core], PERF_EVENT_IOC_ENABLE, 0);
}
void reset_counters(counter_t counters) {
  for(int counter=0; counter<counters->nbperf; counter++)
    for(int core=0; core<counters->nbcores; core++)
      ioctl(counters->counters[counter][core], PERF_EVENT_IOC_RESET, 0);
}

void get_counters(counter_t counters, uint64_t *values) {
  //memset(values, 0, nb_perf*sizeof(long long));
  for(int i=0; i<counters->nbperf; i++) {
    uint64_t accu=0;
    uint64_t count=0;
    for (int core=0; core<counters->nbcores; core++) {
      if (-1 == read(counters->counters[i][core], &count, sizeof(uint64_t))) {
	fprintf(stderr, "PB Lecture resultat");
	exit(EXIT_FAILURE);
      }
      accu += count;
    }
    printf("%lu! \n", accu);

    values[i] = accu;
  }
  //reset_counters(counters);
}
