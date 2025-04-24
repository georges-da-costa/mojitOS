#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include "util.h"

#include <pthread.h>
#include <microhttpd.h>
#include <string.h>

extern FILE *output;

char **__labels;
char prometeus_buffer[1024*1024];
unsigned int delta = 0;
pthread_mutex_t mutex;
struct MHD_Daemon * d;

void end_mutex() {
  pthread_mutex_destroy(&mutex);
  MHD_stop_daemon(d);
}

static enum MHD_Result
internal_server(void * cls,
         struct MHD_Connection * connection,
         const char * url,
         const char * method,
         const char * version,
         const char * upload_data,
         size_t * upload_data_size,
         void ** ptr) {
  UNUSED(url);
  UNUSED(version);
  UNUSED(upload_data);
  static int dummy;
  const char * page = cls;
  struct MHD_Response * response;
  int ret;

  if (0 != strcmp(method, "GET"))
    return MHD_NO; /* unexpected method */

  UNUSED(dummy);
  UNUSED(upload_data_size);
  UNUSED(ptr);
  if (&dummy != *ptr)
    {
      /* The first time only the headers are valid,
         do not respond in the first round... */
      *ptr = &dummy;
      return MHD_YES;
    }

  pthread_mutex_lock(&mutex);
  response = MHD_create_response_from_buffer (delta,
                                              (void*) page,
  					      MHD_RESPMEM_MUST_COPY);
  delta = 0;
  pthread_mutex_unlock(&mutex);
  
  MHD_add_response_header (response, "Content-Type", "text/plain; version=0.0.4");

  ret = MHD_queue_response(connection,
			   MHD_HTTP_OK,
			   response);
  MHD_destroy_response(response);
  return ret;
}



void init_manager(char** labels, int nb_sensors, int stat_mode) {
  UNUSED(nb_sensors);
  UNUSED(stat_mode);
  __labels = labels;
  prometeus_buffer[0] = 0;
  pthread_mutex_init(&mutex, NULL);
  atexit(end_mutex);

  int port = 9999;
  d = MHD_start_daemon(MHD_USE_SELECT_INTERNALLY,
		       port,
		       NULL,
		       NULL,
		       &internal_server,
		       prometeus_buffer,
		       MHD_OPTION_END);
  if(d == NULL)
    exit(EXIT_FAILURE);
}

void use_manager(struct timespec ts,
		 const uint64_t* values, int nb_sensors,
		 uint64_t stat_data) {

  uint64_t timer = ts.tv_sec*1000+ts.tv_nsec/1000000;

  pthread_mutex_lock(&mutex);
  for (int i = 0; i < nb_sensors; i++) {
    /* "PRIu64" is a format specifier to print uint64_t values */
    delta += sprintf(prometeus_buffer+delta, "%s %" PRIu64 " %" PRIu64 "\n", __labels[i], values[i], timer);
  }
  
  if (stat_data != 0) {
    delta += sprintf(prometeus_buffer+delta, "%s %" PRIu64 " %" PRIu64 "\n", "overhead", stat_data, timer);
  }
  pthread_mutex_unlock(&mutex);

}
