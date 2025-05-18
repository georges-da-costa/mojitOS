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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdint.h>
#include <asm/unistd.h>
#include <stdint.h>

#include <likwid.h>

#include "util.h"


struct Likwid {
    int nbcores;
    int gid;
    int nbperf;
  char **labels;
};
typedef struct Likwid Likwid;



void _get_likwid(Likwid *likwid, uint64_t *values)
{
  int err;
  if ((err=perfmon_readCounters()) < 0) {
    printf("Failed to read counters for group %d for thread %d\n",likwid->gid, (-1*err)-1);
    perfmon_finalize();
    topology_finalize();
    exit(1);
  }

  for (int i = 0; i < likwid->nbperf; i++) {
    uint64_t accu = 0;
    for (int core = 0; core < likwid->nbcores; core++) {
      accu += perfmon_getLastResult(likwid->gid, i, core);
    }
    values[i] = accu;
  }
}

void label_likwid(char **labels, void *ptr)
{
    Likwid *state = (Likwid *) ptr;

    for (int i = 0; i < state->nbperf; i++) {
      labels[i] = state->labels[i];
    }
}

void *show_all_likwid(void *none1, size_t none2)
{
  execlp("likwid-perfctr", "likwid-perfctr", "-e", NULL);
  UNUSED(none1);
  UNUSED(none2);
  /* Should not be executed */
  printf("Likwid not available\n");
  exit(EXIT_FAILURE);
  return NULL; /* not reached */
}



unsigned int init_likwid(char *args, void **state)
{
  Likwid *likwid = (Likwid*) malloc(sizeof(struct Likwid));
  likwid->nbperf = 0;
  likwid->labels = NULL;

  char *events = (char*) malloc(sizeof(char)*strlen(args)+1);
  strcpy(events, args);
  while(events != NULL) {
    likwid->nbperf++;
    likwid->labels = (char**) realloc(likwid->labels, sizeof(char*)*likwid->nbperf);
    likwid->labels[likwid->nbperf-1] = events;
    events = index(events, ',');
    if(events != NULL) {
      events[0] = '\0';
      events++;
    }
  }

  if (topology_init() < 0) {
    perror("Failed to initialize LIKWID's topology module\n");
    exit(1);
  }
  CpuTopology_t topology = get_cpuTopology();
  likwid->nbcores = topology->numHWThreads;

  // Create affinity domains. Commonly only needed when reading Uncore counters
  affinity_init();

  int* cpus = (int*)malloc(likwid->nbcores * sizeof(int));
  for (int i=0;i<likwid->nbcores;i++)
    cpus[i] = topology->threadPool[i].apicId;
  // accessClient_setaccessmode(0);
  HPMmode(-1);
  if (perfmon_init(likwid->nbcores, cpus) < 0) {
    topology_finalize();
    perror("perfmon_init");
    exit(1);
  }
  free(cpus);

  // Add eventset string to the perfmon module.
  char env[] = "LIKWID_FORCE=1";
  putenv(env);
  likwid->gid = perfmon_addEventSet(args);
  if (likwid->gid < 0) {
    printf("perfmon_addEventSet with %s\n", args);
    perfmon_finalize();
    topology_finalize();
    exit(1);
  }

  // Setup the eventset identified by group ID (gid).
  if (perfmon_setupCounters(likwid->gid) < 0) {
    perfmon_finalize();
    topology_finalize();
    perror("perfmon_setupCounters");
    exit(1);
  }
  // Start all counters in the previously set up event set.
  if (perfmon_startCounters() < 0) {
    perfmon_finalize();
    topology_finalize();
    perror("perfmon_startCounters");
    exit(1);
  }

  *state = (void *) likwid;

  return likwid->nbperf;
}





void clean_likwid(void *ptr)
{
  Likwid *likwid = (Likwid *) ptr;
  
  free(likwid->labels);
  free(likwid);
  
  perfmon_stopCounters();

  perfmon_finalize();
  affinity_finalize();
  topology_finalize();
}











unsigned int get_likwid(uint64_t *results, void *ptr)
{
    Likwid *state = (Likwid *) ptr;

    _get_likwid(state, results);

    return state->nbperf;
}

