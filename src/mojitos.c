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

#include <assert.h>
#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

typedef struct Opt Opt;
typedef struct Sensor Sensor;
/* optparse typedef */
typedef struct optparse_long Optparse;

struct Sensor {
    initializer_t init;
    getter_t get;
    cleaner_t clean;
    labeler_t label;
    int nb_opt;
};

int nb_defined_sensors = 0;

#include "sensors.h"

Sensor sensors[NB_SENSOR];

#define NB_OPT 5
Optparse opts[NB_OPT + NB_SENSOR_OPT + 1] = {
    {
        .longname = "freq", .shortname = 'f', .argtype = OPTPARSE_REQUIRED,
        .usage_arg = "<freq>",
        .usage_msg = "set amount of measurements per second.",
    },
    {
        .longname = "time", .shortname = 't', .argtype = OPTPARSE_REQUIRED,
        .usage_arg = "<time>",
        .usage_msg = "set duration value (seconds). If 0, then loops infinitely.",
    },
    {
        .longname = "exec", .shortname = 'e', .argtype = OPTPARSE_REQUIRED,
        .usage_arg = "<cmd> ...",
        .usage_msg = "Execute a command with optional arguments.\n"
        "\tIf this option is used, any usage of -t or -f is ignored.",
    },
    {
        .longname = "logfile", .shortname = 'o', .argtype = OPTPARSE_REQUIRED,
        .usage_arg = "<file>",
        .usage_msg = "specify a log file.",
    },
    {
        .longname = "overhead-stats", .shortname = 's', .argtype = OPTPARSE_NONE,
        .usage_arg = NULL,
        .usage_msg = "enable overhead statistics (nanoseconds).",
    },
};

void dumpopt(Optparse *opt)
{
    printf(".It Fl %c | Fl \\-%s", opt->shortname, opt->longname);
    if (opt->usage_arg != NULL) {
        printf(" Ar %s", opt->usage_arg);
    }
    printf("\n");
    printf("%s\n", opt->usage_msg);
}

void dumpopts(Optparse *opts, size_t nb_opt, size_t nb_sensor_opt)
{
    size_t i;

    /* options */
    printf(".Pp\nOPTIONS:\n.Bl -tag -width Ds\n");
    for (i = 0; i < nb_opt; i++) {
        dumpopt(&opts[i]);
    }
    printf(".El\n");

    /* sensors */
    printf(".Pp\nSENSORS:\n.Bl -tag -width Ds\n");
    for (i++; i < nb_opt + nb_sensor_opt; i++) {
        dumpopt(&opts[i]);
    }
    printf(".El\n");
}

void printopt(Optparse *opt)
{
    printf("-%c", opt->shortname);
    printf("|--%s", opt->longname);
    if (opt->usage_arg != NULL) {
        printf(" %s", opt->usage_arg);
    }
    printf("\n\t%s\n", opt->usage_msg);
}

void usage(char **argv)
{
    printf("Usage : %s [OPTIONS] [SENSOR ...] [-e <cmd> ...]\n", argv[0]);

    printf("\nOPTIONS:\n");
    for (int i = 0; i < NB_OPT; i++) {
        printopt(&opts[i]);
    }

    if (nb_defined_sensors == 0) {
        // no captor to show
        exit(EXIT_FAILURE);
    }

    printf("\nSENSORS:\n");
    for (int i = 0; i < NB_SENSOR_OPT; i++) {
        printopt(&opts[NB_OPT + i]);
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
unsigned int nb_sources = 0;
void **states = NULL;
getter_t *getter = NULL;
cleaner_t *cleaner = NULL;

unsigned int nb_sensors = 0;
char **labels = NULL;
uint64_t *values = NULL;

void flushexit(void)
{
    if (output != NULL) {
        fflush(output);
        fclose(output);
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

void add_source(Sensor *cpt, char *arg)
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

    init_sensors(opts, sensors, NB_OPT + NB_SENSOR_OPT, NB_OPT, &nb_defined_sensors);

    if (argc == 1) {
        usage(argv);
    }

    if (argc == 2 && strcmp(argv[1], "--dump-opts") == 0) {
        dumpopts(opts, NB_OPT, NB_SENSOR_OPT);
        exit(EXIT_SUCCESS);
    }

    output = stdout;

    atexit(flushexit);
    signal(SIGTERM, flush);
    signal(SIGINT, flush);

    int opt;
    struct optparse options;
    options.permute = 0;

    optparse_init(&options, argv);
    while ((opt = optparse_long(&options, opts, NULL)) != -1 && application == NULL) {
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
            int opt_idx = NB_OPT;
            for (int i = 0; i < nb_defined_sensors && !ismatch; i++) {
                for (int j = 0; j < sensors[i].nb_opt; j++) {
                    if (opt == opts[opt_idx].shortname) {
                        ismatch = 1;
                        if (opts[opt_idx].fn != NULL) {
                            (void) opts[opt_idx].fn(NULL, 0);
                        } else {
                            add_source(&sensors[i], options.optarg);
                        }
                        break;
                    }
                    opt_idx++;
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

}

