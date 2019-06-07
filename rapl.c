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
#include "rapl.h"

#define MAX_LEN_NAME 100
rapl_t init_rapl(const uint32_t nb_zones, const int *rapl_zones) {
  // get number of processor sockets
  rapl_t rapl = malloc(sizeof(struct _rapl_t));
  rapl->nbzones = nb_zones;
  rapl->zones = rapl_zones;

  rapl->nbpackages = powercap_rapl_get_num_packages();
  if (rapl->nbpackages == 0) {
    perror("no packages found (maybe the kernel module isn't loaded?)");
    exit(-1);
  }
  rapl->pkgs = malloc(rapl->nbpackages * sizeof(powercap_rapl_pkg));  
  for (int package = 0; package < rapl->nbpackages; package++)
    if (powercap_rapl_init(package, &rapl->pkgs[package], 0)) {
      perror("powercap_rapl_init, check access (root needed ?)");
      exit(-1);
    }

  rapl->names = malloc(sizeof(char*)* rapl->nbzones*rapl->nbpackages);
  for (int package = 0; package < rapl->nbpackages; package++) {
    for(int zone=0; zone<rapl->nbzones; zone++) {
      rapl->names[package*rapl->nbzones+zone]=malloc(MAX_LEN_NAME);
      powercap_rapl_get_name(&rapl->pkgs[package], rapl_zones[zone],
			     rapl->names[package*rapl->nbzones+zone], MAX_LEN_NAME);
    }
  }
  return rapl;
}



// values [zone + package *nbzones] microjoules
void get_rapl(uint64_t *values, rapl_t rapl) {
  for (int package = 0; package < rapl->nbpackages; package++) {
    for(int zone=0; zone<rapl->nbzones; zone++) {
      powercap_rapl_get_energy_uj(&rapl->pkgs[package], rapl->zones[zone], &values[package*rapl->nbzones+zone]);
    }
  }
}

void clean_rapl(rapl_t rapl) {
  for (int package = 0; package < rapl->nbpackages; package++) {
    if (powercap_rapl_destroy(&rapl->pkgs[package]))
      perror("powercap_rapl_destroy");
    for (int zone=0; zone<rapl->nbzones; zone++) 
      free(rapl->names[package*rapl->nbzones+zone]);
  }
  free(rapl->names);
  free(rapl->pkgs);
  free(rapl);
}
