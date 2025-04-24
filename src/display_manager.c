#include <time.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>

extern FILE *output;
void init_manager(char** labels, int nb_sensors, int stat_mode) {
  fprintf(output, "#timestamp ");
  
  for (int i = 0; i < nb_sensors; i++) {
    fprintf(output, "%s ", labels[i]);
  }
  
  if (stat_mode == 0) {
    fprintf(output, "overhead ");
  }
  
  fprintf(output, "\n");
}


void use_manager(struct timespec ts,
		 const uint64_t* values, int nb_sensors,
		 uint64_t stat_data) {
  fprintf(output, "%ld.%09ld ", ts.tv_sec, ts.tv_nsec);

  for (int i = 0; i < nb_sensors; i++) {
    /* "PRIu64" is a format specifier to print uint64_t values */
    fprintf(output, "%" PRIu64 " ", values[i]);
  }
  
  if (stat_data != 0) {
    fprintf(output, "%" PRIu64 " ", stat_data);
  }
  
  fprintf(output, "\n");
}
