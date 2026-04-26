/*******************************************************
 Copyright (C) 2026 Georges Da Costa <georges.da-costa@irit.fr>

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
#include <papi.h>
#include <stdint.h>
#include <string.h>
#include "util.h"
#include <inttypes.h>

void *show_all_papi(void *none1, size_t none2)
{
    system("papi_avail");
    UNUSED(none1);
    UNUSED(none2);
    exit(EXIT_SUCCESS);
    return NULL; /* not reached */
}


struct papi_t {
    int *event_set;
    unsigned int nb;
    unsigned int nb_cpu;
    unsigned int numa;
    char **labels;
    long long *values;
    long long *old_values;
};

void clean_papi(void *ptr) {
    struct papi_t *papi = ptr;
    free(papi->values);
    free(papi->old_values);
    free(papi->labels[0]);
    free(papi->labels);
    for (unsigned int cpu=0; cpu<papi->nb_cpu; cpu++)
	PAPI_stop(papi->event_set[cpu], NULL);
    free(papi->event_set);
    free(ptr);
}

// todo
void label_papi(const char **labels, void *ptr) {
    struct papi_t* papi = (struct papi_t*) ptr;
    for(unsigned int i=0; i<papi->nb_cpu*papi->nb; i++)
	labels[i] = papi->labels[i];
}


unsigned int get_papi(uint64_t *results, void *ptr) {
    struct papi_t* papi = (struct papi_t*) ptr;
    
    for (unsigned int cpu=0; cpu<papi->nb_cpu; cpu++) {
	int retval = PAPI_read(papi->event_set[cpu], papi->values+cpu*papi->nb);
	if (retval != PAPI_OK) {
	    fprintf(stderr, "PAPI_read failed.\n");
	    exit(EXIT_FAILURE);
	}
    }
    for (unsigned int i=0; i<papi->nb_cpu*papi->nb; i++) {
	results[i] = papi->values[i] - papi->old_values[i];
	papi->old_values[i] = papi->values[i];
    }
    return papi->nb_cpu*papi->nb;
}



void papi_init_list(char *argv, struct papi_t* papi)
{
    char *token;
    char *save_ptr;
    papi->nb = 0;
    char** labels = NULL;

    char *perf_string = malloc(sizeof(char)*(strlen(argv)+1));
    strcpy(perf_string, argv);
    
    while ((token = strtok_r(perf_string, ",", &save_ptr)) != NULL) {
	perf_string = NULL;

	int event_code;
	if (PAPI_event_name_to_code(token, &event_code) != PAPI_OK) {
	    fprintf(stderr, "Invalid PAPI event name: %s\n", token);
            exit(EXIT_FAILURE);
	}

	for (unsigned int cpu=0; cpu<papi->nb_cpu; cpu++) {
	    int retval = PAPI_add_event(papi->event_set[cpu], event_code);
	    if (retval != PAPI_OK) {
		fprintf(stderr, "PAPI_add_event failed. Error code: %d on cpu %d\n", retval, cpu);
		exit(EXIT_FAILURE);
	    }
	}

	papi->nb += 1;
	labels = realloc(labels, papi->nb * sizeof(char*));
	labels[papi->nb-1] = token;
	
    }
    papi->labels = malloc(sizeof(char*)*papi->nb_cpu*papi->nb);
    for (unsigned int cpu=0; cpu<papi->nb_cpu; cpu++) {
	for (unsigned int cpt=0; cpt<papi->nb; cpt++) {
	    unsigned int current = cpu*papi->nb + cpt;
	    papi->labels[current] = malloc(sizeof(char)*(strlen(labels[cpt])+10));
	    sprintf(papi->labels[current], "%s_%d", labels[cpt], cpu);
	}
    }

    papi->values = (long long*) malloc(sizeof(long long)*papi->nb*papi->nb_cpu);
    papi->old_values = (long long*) malloc(sizeof(long long)*papi->nb*papi->nb_cpu);
}



unsigned int init_papi(char *papi_list, void **papi_struct) {
    const PAPI_hw_info_t *hwinfo;
    struct papi_t* papi = malloc(sizeof(struct papi_t));
    papi->nb = 0;
    papi->labels = NULL;
    
    int retval = PAPI_library_init(PAPI_VER_CURRENT);
    if (retval != PAPI_VER_CURRENT && retval > 0) {
        fprintf(stderr, "PAPI library version mismatch!\n");
	exit(EXIT_FAILURE);
    } else if (retval < 0) {
        fprintf(stderr, "PAPI library init error!\n");
	exit(EXIT_FAILURE);	
    }    

    hwinfo = PAPI_get_hardware_info();
    if (hwinfo == NULL) {
        fprintf(stderr, "Failed to get hardware info\n");
        exit(EXIT_FAILURE);
    }
    papi->nb_cpu = hwinfo->ncpu * hwinfo->nnodes;

    papi->event_set = malloc(sizeof(int)*papi->nb_cpu);

    PAPI_set_cmp_granularity(PAPI_GRN_SYS, 0);

    
    for (unsigned cpu=0; cpu<papi->nb_cpu; cpu++) {
	papi->event_set[cpu] = PAPI_NULL;
    
	if (PAPI_create_eventset(&papi->event_set[cpu]) != PAPI_OK) {
	    fprintf(stderr, "PAPI_create_eventset failed.\n");
	    exit(EXIT_FAILURE);	
	}

        retval = PAPI_assign_eventset_component(papi->event_set[cpu], 0);
        if (retval != PAPI_OK) {
            fprintf(stderr, "Error assigning component for CPU %d: %s\n", cpu, PAPI_strerror(retval));
            // Don't exit, might not be necessary depending on PAPI version
        }

        PAPI_option_t opt;
        memset(&opt, 0, sizeof(opt));
        opt.cpu.eventset = papi->event_set[cpu];
        opt.cpu.cpu_num = cpu;

        retval = PAPI_set_opt(PAPI_CPU_ATTACH, &opt);
        if (retval != PAPI_OK) {
            fprintf(stderr, "Error attaching to CPU %d: %s\n", cpu, PAPI_strerror(retval));
            exit(1);
        }
    }

	
    papi_init_list(papi_list, papi);
    
    for (unsigned cpu=0; cpu<papi->nb_cpu; cpu++) {
	if (PAPI_start(papi->event_set[cpu]) != PAPI_OK) {
	    fprintf(stderr, "PAPI_start failed.\n");
	    exit(EXIT_FAILURE);
	}
    }
    
    *papi_struct = papi;
    return papi->nb_cpu * papi->nb;

}
