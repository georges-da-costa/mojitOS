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

typedef struct Opt Opt;
typedef struct Sensor Sensor;
/* optparse typedef */
typedef struct optparse_long Optparse;

#include "libmojitos.h"

#include "manager.h"

#define NB_OPT 4
Optparse opts[NB_OPT + 1] = {
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
        .longname = "option", .shortname = 'o', .argtype = OPTPARSE_REQUIRED,
        .usage_arg = "<output file> or <port number>",
        .usage_msg = "specify a log file for MojitO/S or a port number for prometeus_mojitO/S.",
    },
    {
        .longname = "overhead-stats", .shortname = 's', .argtype = OPTPARSE_NONE,
        .usage_arg = NULL,
        .usage_msg = "enable overhead statistics (nanoseconds).",
    },
};

void _dumpopts(Optparse *opt, size_t nb)
{
  for(size_t i=0; i<nb; i++) {
    printf(".It Fl %c | Fl \\-%s", opt[i].shortname, opt[i].longname);
    if (opt[i].usage_arg != NULL) {
      printf(" Ar %s", opt[i].usage_arg);
    }
    printf("\n");
    printf("%s\n", opt[i].usage_msg);
  }
}

extern Optparse _moj_opts[];
extern int nb_defined_options;
void dumpopts(Optparse *opts, size_t nb_opt, size_t nb_sensor_opt)
{
    /* options */
    printf(".Pp\nOPTIONS:\n.Bl -tag -width Ds\n");
    _dumpopts(opts, nb_opt);
    printf(".El\n");
    
    /* sensors */
    printf(".Pp\nSENSORS:\n.Bl -tag -width Ds\n");
    _dumpopts(_moj_opts, nb_sensor_opt);
    printf(".El\n");
}

void printopts(Optparse *opt, size_t nb)
{
  for(size_t i=0; i<nb; i++) {
    printf("-%c", opt[i].shortname);
    printf("|--%s", opt[i].longname);
    if (opt[i].usage_arg != NULL) {
        printf(" %s", opt[i].usage_arg);
    }
    printf("\n\t%s\n", opt[i].usage_msg);
  }
}

void usage(char *name)
{
    printf("Usage : %s [OPTIONS] [SENSOR ...] [-- <cmd> <argument>...]\n", name);

    printf("\nOPTIONS:\n");
    printopts(opts, NB_OPT);

    if (nb_defined_options == 0) // no sensor to show
      return;

    printf("\nSENSORS:\n");
    printopts(_moj_opts, nb_defined_options);
}

void flush(int none)
{
    UNUSED(none);
    exit(0);
}

FILE *output;
char *output_option=NULL;

void flushexit(void)
{
  if (output != NULL) {
    fflush(output);
    fclose(output);
  }
  moj_clean();
}


int main(int argc, char **argv)
{
    int total_time = 1;
    int delta = 0;
    int frequency = 1;
    char **application = NULL;
    int stat_mode = -1;
 
    output = stdout;

    atexit(flushexit);
    signal(SIGTERM, flush);
    signal(SIGINT, flush);

    char **save = malloc((argc+1)*sizeof(char*));
    memcpy(save, argv, (argc+1)*sizeof(char*));
    
    int opt;
    struct optparse options;
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
	  output_option = options.optarg;
            break;
        default:
  	    break;
        }
    }

    for(int pos=0; pos < argc-1; pos++) {
      if(strcmp("--", save[pos]) == 0) {
	application = &argv[pos+1];
	signal(17, flush);
	break;
      }
    }

    int nb_sensors = moj_init(save);
    free(save);
    
    if (argc == 1) {
        usage(argv[0]);
	exit(EXIT_FAILURE);
    }

    if (argc == 2 && strcmp(argv[1], "--dump-opts") == 0) {
        dumpopts(opts, NB_OPT, nb_defined_options);
        exit(EXIT_SUCCESS);
    }

    setvbuf(output, NULL, _IONBF, BUFSIZ);
    struct timespec ts;
    struct timespec ts_ref;

    char** labels = moj_labels();
    init_manager(labels, nb_sensors, stat_mode);
		 
    uint64_t stat_data = 0;

    if (application != NULL) {
      if (fork() == 0) {
	execvp(application[0], application);
	exit(0);
      }
    }
    
    for (int temps = 0; temps < total_time * frequency; temps += delta) {
        clock_gettime(CLOCK_REALTIME, &ts_ref);

        // Get Data
	const uint64_t* values = moj_get_values();

	if (stat_mode == 0) {
	  clock_gettime(CLOCK_REALTIME, &ts);
	  
	  if (ts.tv_nsec >= ts_ref.tv_nsec) {
	    stat_data = ts.tv_nsec - ts_ref.tv_nsec;
	  } else {
	    stat_data = 1000 * 1000 * 1000 + ts.tv_nsec - ts_ref.tv_nsec;
	  }
	}

	use_manager(ts_ref, values, nb_sensors, stat_data);

        clock_gettime(CLOCK_REALTIME, &ts);
        usleep(1000 * 1000 / frequency - (ts.tv_nsec / 1000) % (1000 * 1000 / frequency));
    }

}

