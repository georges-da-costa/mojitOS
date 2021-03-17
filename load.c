/*******************************************************
 Copyright (C) 2019-2020 Georges Da Costa <georges.da-costa@irit.fr>

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
#include <stdint.h>

#define LOAD_BUFFER_SIZE 1024
char buffer[LOAD_BUFFER_SIZE];

static int load_fid=-1;

void init_load() {
  load_fid = open("/proc/stat", O_RDONLY);
}

void get_load(uint64_t* results) {
  pread(load_fid, buffer, LOAD_BUFFER_SIZE-1, 0);
  int pos=0;
  while(buffer[pos] > '9' || buffer[pos] < '0') pos++;
  for(int i=0; i<10; i++) {
    results[i] = strtoull(buffer+pos, NULL, 10);
    while(buffer[pos] <= '9' && buffer[pos] >= '0') pos++;
    pos++;
  }
}

void clean_load() {
  close(load_fid);
}

