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

#include <inttypes.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#include "counters.h"
#include "rapl.h"
#include "infiniband.h"
#include "load.h"
#include "network.h"
#include "temperature.h"

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"

#define UNUSED(expr) do { (void)(expr); } while (0)
#define PANIC(code, fmt, ...)  				 \
	do {									 \
		fprintf(stderr, "Exit on error: ");  \
		fprintf(stderr, fmt, ##__VA_ARGS__); \
		fprintf(stderr, "\n"); 			     \
		exit(code); 						 \
	} while (0)


void usage(char **argv)
{
    printf("Usage : %s [-rlucs] [-t time] [-f freq] [-p perf_list] [-d network_device]\n"
           "                    [-i infiniband_path] [-o logfile] [-e command arguments...]\n"
           "if time==0 then loops infinitively\n"
           "if -e is present, time and freq are not used\n"
           "-r activates RAPL\n"
           "-p activates performance counters\n"
           "   perf_list is coma separated list of performance counters without space. Ex: instructions,cache_misses\n"
           "-l lists the possible performance counters and quits\n"
           "-d activates network monitoring (if network_device is X, tries to detect it automatically)\n"
           "-i activates infiniband monitoring (if infiniband_path is X, tries to detect it automatically)\n"
           "-s activates statistics of overhead in nanoseconds\n"
           "-u activates report of system load\n"
           "-c activates report of processor temperature\n"
           , argv[0]);
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

typedef unsigned int (initializer_t)(char *, void **);
typedef void (labeler_t)(char **, void *);
typedef unsigned int (*getter_t)(uint64_t *, void *);
typedef void (*cleaner_t)(void *);

unsigned int nb_sources = 0;
void **states = NULL;
getter_t *getter = NULL;
cleaner_t *cleaner = NULL;

unsigned int nb_sensors = 0;
char **labels = NULL;
uint64_t *values = NULL;

void add_source(initializer_t init, char *arg, labeler_t labeler,
                getter_t get, cleaner_t clean)
{
    nb_sources++;
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

    if (argc == 1) {
        usage(argv);
    }

    output = stdout;

    atexit(flushexit);
    signal(15, flush);

    int opt;
    struct optparse options;
    options.permute = 0;
    struct optparse_long longopts[] = {
        {"monitor-infiniband", 'i', OPTPARSE_REQUIRED},
        {"freq", 'f', OPTPARSE_REQUIRED},
        {"time", 't', OPTPARSE_REQUIRED},
        {"net-dev", 'd', OPTPARSE_REQUIRED},
        {"exec", 'e', OPTPARSE_REQUIRED},
        {"logfile", 'o', OPTPARSE_REQUIRED},
        {"perf-list", 'p', OPTPARSE_REQUIRED},
        {"cpu-temp", 'c', OPTPARSE_NONE},
        {"rapl", 'r', OPTPARSE_NONE},
        {"overhead-stats", 's', OPTPARSE_NONE},
        {"sysload", 'u', OPTPARSE_NONE},
        {"list", 'l', OPTPARSE_NONE},
    };

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
        case 'd':
            add_source(init_network, options.optarg, label_network, get_network, clean_network);
            break;
        case 'i':
            add_source(init_infiniband, options.optarg, label_infiniband, get_network, clean_network);
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
        case 'p':
            add_source(init_counters, options.optarg, label_counters, get_counters, clean_counters);
            break;
        case 'r':
            add_source(init_rapl, NULL, label_rapl, get_rapl, clean_rapl);
            break;
        case 'u':
            add_source(init_load, NULL, label_load, get_load, clean_load);
            break;
        case 'c':
            add_source(init_temperature, NULL, label_temperature, get_temperature, clean_temperature);
            break;
        case 's':
            stat_mode = 0;
            break;
        case 'l':
            show_all_counters();
            exit(EXIT_SUCCESS);
        default:
            fprintf(stderr, "%s: %s\n", argv[0], options.errmsg);
            usage(argv);
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

