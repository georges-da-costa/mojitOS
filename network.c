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
    along with Foobar.  If not, see <https://www.gnu.org/licenses/>.

 *******************************************************/

#include <stdio.h>
#include <stdlib.h>

char **init_network(char* dev) {
  if(dev==NULL)
    return NULL;
  char *filenames[] = {"/sys/class/net/%s/statistics/rx_packets",
		       "/sys/class/net/%s/statistics/rx_bytes",
		       "/sys/class/net/%s/statistics/tx_packets",
		       "/sys/class/net/%s/statistics/tx_bytes"};
  char** sources = malloc(sizeof(char*)*4);
  for(int i=0; i<4; i++) {
    sources[i] = malloc(200);
    sprintf(sources[i], filenames[i], dev);
  }

  return sources;
}

void get_network(long long* results, char** sources) {
  if(sources==NULL)
    return;
  for(int i=0; i<4; i++){
    FILE* f = fopen(sources[i], "rb");
    fscanf(f, "%lld", &results[i]);
    fclose(f);
  }
}

void clean_network(char **sources) {
  if(sources==NULL)
    return;
  for(int i=0;i<4;i++)
    free(sources[i]);
  free(sources);
}    
