/*******************************************************
 Copyright (C) 2018-2020 Georges Da Costa <georges.da-costa@irit.fr>

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
    along with Foobar.  If not, see <https://www.gnu.org/licenses/>.

 *******************************************************/
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

#include <glob.h>

int *init_infiniband(char* infi_path) {
  if(infi_path==NULL)
    return NULL;

  if(strcmp(infi_path,"X")==0) {

    glob_t res;
  
    glob("/sys/class/infiniband/*/ports/*/counters/", 0, NULL, &res);
    if(res.gl_pathc == 0)
      return NULL;
    infi_path = res.gl_pathv[0];
  }
  
  char *filenames[] = {"%s/port_rcv_packets",
		       "%s/port_rcv_data",
		       "%s/port_xmit_packets",
		       "%s/port_xmit_data"};
  int* sources = malloc(sizeof(int)*4);
  char buffer[1024];
  for(int i=0; i<4; i++) {
    sprintf(buffer, filenames[i], infi_path);
    sources[i] = open(buffer, O_RDONLY);
  }

  return sources;
}

/* void get_network(long long* results, char** sources) { */
/*   if(sources==NULL) */
/*     return; */
/*   for(int i=0; i<4; i++){ */
/*     FILE* f = fopen(sources[i], "rb"); */
/*     fscanf(f, "%lld", &results[i]); */
/*     fclose(f); */
/*   } */
/* } */

/* void clean_network(char **sources) { */
/*   if(sources==NULL) */
/*     return; */
/*   for(int i=0;i<4;i++) */
/*     free(sources[i]); */
/*   free(sources); */
/* }     */
