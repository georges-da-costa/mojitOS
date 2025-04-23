#include <stdint.h>
#include <stdlib.h>
#include <assert.h>

#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include "optparse.h"

#include "libmojitos.h"

typedef unsigned int (*initializer_t)(char *, void **);
typedef void (*labeler_t)(char **, void *);
typedef unsigned int (*getter_t)(uint64_t *, void *);
typedef void (*cleaner_t)(void *);

struct Sensor {
    initializer_t init;
    getter_t get;
    cleaner_t clean;
    labeler_t label;
    int nb_opt;
};
typedef struct Sensor Sensor;

int nb_defined_sensors = 0;
int nb_defined_options = 0;

typedef struct optparse_long Optparse;

#include "sensors.h"


Sensor sensors[NB_SENSOR];
Optparse _moj_opts[NB_SENSOR_OPT + 1];
  
unsigned int nb_sources = 0;
unsigned int nb_sensors = 0; // might have multiple sensors per source
char **labels = NULL;
uint64_t *values = NULL;
void **states = NULL;
getter_t *getter = NULL;
cleaner_t *cleaner = NULL;

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

int moj_init(char **argv) {
  init_sensors(_moj_opts, sensors, NB_SENSOR_OPT, 0, &nb_defined_sensors);
  nb_defined_options = NB_SENSOR_OPT;
  
  int opt;
  struct optparse options;
  optparse_init(&options, argv);

  while ((opt = optparse_long(&options, _moj_opts, NULL)) != -1) {
    int ismatch = 0;
    int opt_idx = 0;
    for (int s_idx = 0; s_idx < NB_SENSOR && !ismatch; s_idx++) {
      for (int j = 0; j < sensors[s_idx].nb_opt; j++) {

	if (opt == _moj_opts[opt_idx].shortname) {
	  ismatch = 1;
	  if (_moj_opts[opt_idx].fn != NULL) {
	    (void) _moj_opts[opt_idx].fn(NULL, 0);
	  } else {
	    add_source(&sensors[s_idx], options.optarg);
	  }
	  break;
	}
	opt_idx++;
      }
    }
  }

  return nb_sensors;
}

void moj_clean() {
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

char **moj_labels() {
  return labels;
}

uint64_t* moj_get_values() {
  unsigned int current = 0;

  for (unsigned int i = 0; i < nb_sources; i++) {
    current += getter[i](&values[current], states[i]);
  }
  return values;
}

