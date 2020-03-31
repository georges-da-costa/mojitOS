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
#include <stdio.h>
#include <stdlib.h>

#define LOAD_BUFFER_SIZE 1024
char buffer[LOAD_BUFFER_SIZE];

void get_load(long long* results) {
  FILE* f = fopen("/proc/stat", "rb");
  fgets(buffer, LOAD_BUFFER_SIZE, f);
  fclose(f);
  int pos=0;
  while(buffer[pos] > '9' || buffer[pos] < '0') pos++;
  for(int i=0; i<10; i++) {
    results[i] = atoll(buffer+pos);
    while(buffer[pos] <= '9' && buffer[pos] >= '0') pos++;
    pos++;
  }
}
