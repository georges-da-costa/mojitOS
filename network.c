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
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int *init_network(char* dev) {
  if(dev==NULL)
    return NULL;

  if(strcmp(dev,"X")==0) {
    int f = open("/proc/net/route", O_RDONLY);
    char buffer[1000];
    read(f, buffer, 999);
    char *start_of_dev = index(buffer, '\n')+1;
    char *end_of_dev = index(start_of_dev, '\t');
    *end_of_dev='\0';
    dev = start_of_dev;
    close(f);
  }
  
  char *filenames[] = {"/sys/class/net/%s/statistics/rx_packets",
		       "/sys/class/net/%s/statistics/rx_bytes",
		       "/sys/class/net/%s/statistics/tx_packets",
		       "/sys/class/net/%s/statistics/tx_bytes"};
  int* sources = malloc(sizeof(int)*4);
  char buffer2[256];
  for(int i=0; i<4; i++) {
    sprintf(buffer2, filenames[i], dev);
    sources[i] = open(buffer2, O_RDONLY);
  }

  return sources;
}

void get_network(long long* results, int *sources) {
  if(sources==NULL)
    return;
  char buffer[128];
  for(int i=0; i<4; i++){
    pread(sources[i], buffer, 127, 0);
    results[i] = atoll(buffer);
  }
}

void clean_network(int *sources) {
  if(sources==NULL)
    return;
  for(int i=0;i<4;i++)
    close(sources[i]);
  free(sources);
}    
