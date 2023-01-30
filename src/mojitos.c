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

#include <assert.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "util.h"

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"

typedef unsigned int (*initializer_t)(char *, void **);
typedef void (*labeler_t)(char **, void *);
typedef unsigned int (*getter_t)(uint64_t *, void *);
typedef void (*cleaner_t)(void *);

struct captor {
    char *usage_arg;
    char *usage_msg;
    initializer_t init;
    getter_t get;
    cleaner_t clean;
    labeler_t label;
};

int nb_defined_captors = 0;

#include "captors.h"

struct captor captors[NB_CAPTORS];

#define NB_OPTS 6
struct optparse_long longopts[NB_OPTS + NB_CAPTORS + 1] = {
    {"overhead-stats", 's', OPTPARSE_NONE},
    {"list", 'l', OPTPARSE_NONE},
    {"freq", 'f', OPTPARSE_REQUIRED},
    {"time", 't', OPTPARSE_REQUIRED},
    {"exec", 'e', OPTPARSE_REQUIRED},
    {"logfile", 'o', OPTPARSE_REQUIRED},
};


void usage(char **argv)
{
    printf("Usage : %s [OPTIONS] [CAPTOR ...] [-o logfile] [-e cmd ...]\n"
           "\nOPTIONS:\n"
           "-t <time>\t\tspecify time\n"
           "-f <freq>\t\tspecify frequency\n"
           "-e <cmd>\t\tspecify a command\n"
           "-l\t\tlist the possible performance counters and quit\n"
           "-s\t\tenable overhead statistics in nanoseconds\n"
           "if time==0 then loops infinitively\n"
           "if -e is present, time and freq are not used\n"
           , argv[0]);

    if (nb_defined_captors == 0) {
        // no captor to show
        exit(EXIT_FAILURE);
    }

    printf("\nCAPTORS:\n");

    for (int i = 0; i < nb_defined_captors; i++) {
        printf("-%c", longopts[NB_OPTS + i].shortname);
        if (captors[i].usage_arg != NULL) {
            printf(" %s", captors[i].usage_arg);
        }
        printf("\n\t%s\n", captors[i].usage_msg);
    }

    exit(EXIT_FAILURE);
}

void sighandler(int none)
{
    UNUSED(none);
}

void flush(int none)
{
    UNUSED(none);
    exit(0);
}

FILE *output;
void flushexit()
{
    if (output != NULL) {
        fflush(output);
        fclose(output);
    }
}

unsigned int nb_sources = 0;
void **states = NULL;
getter_t *getter = NULL;
cleaner_t *cleaner = NULL;

unsigned int nb_sensors = 0;
char **labels = NULL;
uint64_t *values = NULL;

void add_source(struct captor *cpt, char *arg)
{
    nb_sources++;
    initializer_t init = cpt->init;
    labeler_t labeler = cpt->label;
    getter_t get = cpt->get;
    cleaner_t clean = cpt->clean;

    states = realloc(states, nb_sources * sizeof(void *));
    int nb = init(arg, &states[nb_sources - 1]);

    if (nb == 0) {
        nb_sources--;
        states = realloc(states, nb_sources * sizeof(void *));
        return;
    }

    getter = realloc(getter, nb_sources * sizeof(void *));
    getter[nb_sources - 1] = get;
    cleaner = realloc(cleaner, nb_sources * sizeof(void *));
    cleaner[nb_sources - 1] = clean;

    labels = realloc(labels, (nb_sensors + nb) * sizeof(char *));
    labeler(labels + nb_sensors, states[nb_sources - 1]);

    values = realloc(values, (nb_sensors + nb) * sizeof(uint64_t));
    nb_sensors += nb;
}

int main(int argc, char **argv)
{
    int total_time = 1;
    int delta = 0;
    int frequency = 1;
    char **application = NULL;
    int stat_mode = -1;

    init_captors(longopts, captors, NB_OPTS + NB_CAPTORS, NB_OPTS, &nb_defined_captors);

    if (argc == 1) {
        usage(argv);
    }

    output = stdout;

    atexit(flushexit);
    signal(15, flush);

    int opt;
    struct optparse options;
    options.permute = 0;

    optparse_init(&options, argv);
    while ((opt = optparse_long(&options, longopts, NULL)) != -1 && application == NULL) {
        switch (opt) {
        case 'f':
            frequency = atoi(options.optarg);
            break;
        case 't':
            total_time = atoi(options.optarg);
            delta = 1;
            if (total_time == 0) {
                total_time = 1;
                delta = 0;
            }
            break;
        case 's':
            stat_mode = 0;
            break;
        case 'l':
            show_all_counters();
            exit(EXIT_SUCCESS);
        case 'o':
            if ((output = fopen(options.optarg, "wb")) == NULL) {
                perror("fopen");
                PANIC(1, "-o %s", options.optarg);
            }
            break;
        case 'e':
            application = options.argv;
            signal(17, sighandler);
            break;
        default: {
            int ismatch = 0;
            for (int i = 0; i < nb_defined_captors && !ismatch; i++) {
                if (opt == longopts[NB_OPTS + i].shortname) {
                    ismatch = 1;
                    add_source(&captors[i], options.optarg);
                }
            }
            if (!ismatch) {
                fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
                usage(argv);
            }
        }
        }
    }

    setvbuf(output, NULL, _IONBF, BUFSIZ);
    struct timespec ts;
    struct timespec ts_ref;

    fprintf(output, "#timestamp ");

    for (unsigned int i = 0; i < nb_sensors; i++) {
        fprintf(output, "%s ", labels[i]);
    }

    if (stat_mode == 0) {
        fprintf(output, "overhead ");
    }

    fprintf(output, "\n");

    unsigned long int stat_data = 0;

    for (int temps = 0; temps < total_time * frequency; temps += delta) {
        clock_gettime(CLOCK_MONOTONIC, &ts_ref);

        // Get Data
        unsigned int current = 0;

        for (unsigned int i = 0; i < nb_sources; i++) {
            current += getter[i](&values[current], states[i]);
        }

        if (application != NULL) {

            if (fork() == 0) {
                execvp(application[0], application);
                exit(0);
            }

            pause();
            clock_gettime(CLOCK_MONOTONIC, &ts);

            if (ts.tv_nsec >= ts_ref.tv_nsec) {
                fprintf(output, "%ld.%09ld ", (ts.tv_sec - ts_ref.tv_sec), ts.tv_nsec - ts_ref.tv_nsec);
            } else {
                fprintf(output, "%ld.%09ld ", (ts.tv_sec - ts_ref.tv_sec) - 1, 1000 * 1000 * 1000 + ts.tv_nsec - ts_ref.tv_nsec);
            }
        } else {
#ifdef DEBUG
            clock_gettime(CLOCK_MONOTONIC, &ts);
            fprintf(stderr, "%ld\n", (ts.tv_nsec - ts_ref.tv_nsec) / 1000);
            //Indiv: mean: 148 std: 31 % med: 141 std: 28 %
            //Group: mean: 309 std: 41 % med: 297 std: 39 %
#endif

            if (stat_mode == 0) {
                clock_gettime(CLOCK_MONOTONIC, &ts);

                if (ts.tv_nsec >= ts_ref.tv_nsec) {
                    stat_data = ts.tv_nsec - ts_ref.tv_nsec;
                } else {
                    stat_data = 1000 * 1000 * 1000 + ts.tv_nsec - ts_ref.tv_nsec;
                }
            }

            // Treat Data
            fprintf(output, "%ld.%09ld ", ts_ref.tv_sec, ts_ref.tv_nsec);
        }

        for (unsigned int i = 0; i < nb_sensors; i++) {
            /* "PRIu64" is a format specifier to print uint64_t values */
            fprintf(output, "%" PRIu64 " ", values[i]);
        }

        if (stat_mode == 0) {
            fprintf(output, "%ld ", stat_data);
        }

        fprintf(output, "\n");

        if (application != NULL) {
            break;
        }

        clock_gettime(CLOCK_MONOTONIC, &ts);
        usleep(1000 * 1000 / frequency - (ts.tv_nsec / 1000) % (1000 * 1000 / frequency));
    }

    for (unsigned int i = 0; i < nb_sources; i++) {
        cleaner[i](states[i]);
    }

    if (nb_sources > 0) {
        free(getter);
        free(cleaner);
        free(labels);
        free(values);
        free(states);
    }
}

