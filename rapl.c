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
#include <string.h>

#include "rapl.h"

#define MAX_LEN_NAME 100
_rapl_t* init_rapl(const uint32_t nb_zones, const int *rapl_zones) {
  // get number of processor sockets
  _rapl_t* rapl = malloc(sizeof(struct _rapl_t));
  rapl->nb = 0;
  rapl->packages = NULL;
  rapl->zones = NULL;

  rapl->nb_pkgs = powercap_rapl_get_num_packages();
  if (rapl->nb_pkgs == 0) {
    perror("no packages found (maybe the kernel module isn't loaded?)");
    exit(-1);
  }
  rapl->pkgs = malloc(rapl->nb_pkgs * sizeof(powercap_rapl_pkg));  
  for (int package = 0; package < rapl->nb_pkgs; package++)
    if (powercap_rapl_init(package, &rapl->pkgs[package], 0)) {
      perror("powercap_rapl_init, check access (root needed ?)");
      exit(-1);
    }

  printf("%d %d\n\n", nb_zones, rapl->nb_pkgs);
  rapl->names = NULL;
    
  char _name[MAX_LEN_NAME+1];
  char _name2[MAX_LEN_NAME+10];

  for (unsigned int package = 0; package < rapl->nb_pkgs; package++) {
    for(unsigned int zone=0; zone < nb_zones; zone++) {
      int a=powercap_rapl_get_name(&rapl->pkgs[package], rapl_zones[zone],
			     _name, MAX_LEN_NAME);
      if (a>0) {

	sprintf(_name2, "%s%u", _name, package);

	rapl->nb++;
	rapl->names = realloc(rapl->names, sizeof(char*)*rapl->nb);
	rapl->names[rapl->nb-1] = malloc(sizeof(char) * (strlen(_name2)+1));
	rapl->zones = realloc(rapl->zones, sizeof(uint32_t)*rapl->nb);
	rapl->packages = realloc(rapl->packages, sizeof(uint32_t)*rapl->nb);
	
	strcpy(rapl->names[rapl->nb-1], _name2);
	rapl->zones[rapl->nb-1] = zone;
	rapl->packages[rapl->nb-1] = package;
      }
      printf("%d %d %d %s\n\n", a, package, zone, _name2);
    }
  }
  return rapl;
}



// values [zone + package *nbzones] microjoules
void get_rapl(uint64_t *values, _rapl_t* rapl) {
  for (int i = 0; i < rapl->nb; i++) {
    powercap_rapl_get_energy_uj(&rapl->pkgs[rapl->packages[i]],
				rapl->zones[i],
				&values[i]);
  }
}

void clean_rapl(_rapl_t* rapl) {
  for (int package = 0; package < rapl->nb_pkgs; package++)
    if (powercap_rapl_destroy(&rapl->pkgs[package]))
      perror("powercap_rapl_destroy");
  for (int elem=0; elem<rapl->nb; elem++) 
      free(rapl->names[elem]);
  
  free(rapl->names);
  free(rapl->pkgs);
  free(rapl->zones);
  free(rapl->packages);
  free(rapl);
}
