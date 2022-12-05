/*******************************************************
 Copyright (C) 2022-2023 Georges Da Costa <georges.da-costa@irit.fr>

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
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <inttypes.h>


#define MAX_HEADER 128

char *get_frapl_string(const char *filename) {
  int fd = open(filename, O_RDONLY);
  if( fd == -1)
    return NULL;
  char *result = malloc(MAX_HEADER);
  int nb = read(fd, result, MAX_HEADER);
  close(fd);
  result[nb-1] = 0;
  return (result);
}  

void test_append(char* name, int i) {
  //char last = name[strlen(name)-1];
  //if (last>='0' && last <= '9')
  //  return;
  sprintf(name+strlen(name), "%d", i);
}


struct _frapl_t {
  unsigned int nb;
  char **names;
  int *fids;
  uint64_t* values;
  uint64_t* tmp_values;

};
typedef struct _frapl_t _frapl_t;


void add_frapl_source(_frapl_t* rapl, char*name, char*energy_uj) {
  rapl->nb += 1;
  rapl->names = realloc(rapl->names, sizeof(char**)*rapl->nb);
  rapl->fids = realloc(rapl->fids, sizeof(int*)*rapl->nb);
  
  rapl->names[rapl->nb-1] = malloc(strlen(name)+1);
  strcpy(rapl->names[rapl->nb-1], name);
  printf("%s\n", energy_uj);

  rapl->fids[rapl->nb-1] = open(energy_uj, O_RDONLY);
}


void _get_frapl(uint64_t *values, _frapl_t* rapl) {
  static char buffer[512];

  for (int i = 0; i < rapl->nb; i++) {

    pread(rapl->fids[i], buffer, 100, 0);
    values[i] = strtoull(buffer, NULL, 10);
  }
}


unsigned int init_frapl(char* none, void **ptr) {
  _frapl_t *rapl = malloc(sizeof(_frapl_t));
  rapl->nb = 0;
  rapl->names = NULL;
  rapl->fids = NULL;

  char buffer[1024];
  char *name_base = "/sys/devices/virtual/powercap/intel-rapl/intel-rapl:%d/%s";
  char *name_sub = "/sys/devices/virtual/powercap/intel-rapl/intel-rapl:%d/intel-rapl:%d:%d/%s";

  for (int i=0;; i++) {
    sprintf(buffer, name_base, i, "name");
    char *tmp = get_frapl_string(buffer);
    if (tmp == NULL) break;
    printf("%s\n", tmp);
    test_append(tmp, i);
    printf("%s -> %s\n", buffer, tmp);

    sprintf(buffer, name_base, i, "energy_uj");
    add_frapl_source(rapl, tmp, buffer);
    free(tmp);

    for (int j=0;; j++) {
      sprintf(buffer, name_sub, i, i, j, "name");
      char *tmp_sub = get_frapl_string(buffer);
      if (tmp_sub == NULL) break;
      printf("%s\n", tmp_sub);
      test_append(tmp_sub, i);
      printf("%s -> %s\n", buffer, tmp_sub);


      sprintf(buffer, name_sub, i, i, j, "energy_uj");
      add_frapl_source(rapl, tmp_sub, buffer);
      
      free(tmp_sub);
    }
  }

  rapl->values = calloc(sizeof(uint64_t), rapl->nb);
  rapl->tmp_values = calloc(sizeof(uint64_t), rapl->nb);

  _get_frapl(rapl->values, rapl);

  *ptr = (void*)rapl;
  return rapl->nb;
}


unsigned int get_frapl(uint64_t* results, void* ptr) {
  _frapl_t* state = (_frapl_t*) ptr;
  _get_frapl(state->tmp_values, state);
  for(int i=0; i<state->nb; i++)
    results[i] = state->tmp_values[i] - state->values[i];

  memcpy(state->values, state->tmp_values, sizeof(uint64_t)*state->nb);
  return state->nb;
}

void clean_frapl(void *ptr) {
  _frapl_t* rapl = (_frapl_t*) ptr;
  for(int i=0; i<rapl->nb; i++) {
    free(rapl->names[i]);
    close(rapl->fids[i]);
  }
  free(rapl->names);
  free(rapl->fids);
  free(rapl->values);
  free(rapl->tmp_values);
  free(rapl);
}


void label_frapl(char **labels, void *ptr) {
  _frapl_t* rapl = (_frapl_t*) ptr;
  for(int i=0; i<rapl->nb; i++)
    labels[i] = rapl->names[i];
}


/*








#define MAX_LEN_NAME 100

// values [zone + package *nbzones] microjoules
void _get_rapl(uint64_t *values, _rapl_t* rapl) {
  for (int i = 0; i < rapl->nb; i++) {
#ifdef DEBUG
    int ret =
#endif
      powercap_rapl_get_energy_uj(&rapl->pkgs[rapl->packages[i]],
				rapl->zones[i],
				&values[i]);
#ifdef DEBUG
    printf("GETRAPL: package %d, zone %d, name %s, ret: %d\n", rapl->packages[i], rapl->zones[i], rapl->names[i], ret);
#endif
  }
}

unsigned int init_rapl(char* none, void **ptr) {
  // get number of processor sockets
  _rapl_t* rapl= malloc(sizeof(struct _rapl_t));
  rapl->nb = 0;
  rapl->packages = NULL;
  rapl->zones = NULL;

  rapl->nb_pkgs = powercap_rapl_get_num_instances();
  //rapl->nb_pkgs = powercap_rapl_get_num_packages();
  
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

  rapl->names = NULL;
    
  char _name[MAX_LEN_NAME+1];
  char _name2[MAX_LEN_NAME+11];

  for (unsigned int package = 0; package < rapl->nb_pkgs; package++) {
    for(unsigned int zone=0; zone < nb_zones; zone++) {
      int length=powercap_rapl_get_name(&rapl->pkgs[package], rapl_zones[zone],
			     _name, MAX_LEN_NAME);
      if (length>0) {

	sprintf(_name2, "%s%u", _name, package);

	rapl->nb++;
	rapl->names = realloc(rapl->names, sizeof(char*)*rapl->nb);
	rapl->names[rapl->nb-1] = malloc(sizeof(char) * (strlen(_name2)+1));
	rapl->zones = realloc(rapl->zones, sizeof(uint32_t)*rapl->nb);
	rapl->packages = realloc(rapl->packages, sizeof(uint32_t)*rapl->nb);
	
	strcpy(rapl->names[rapl->nb-1], _name2);
	rapl->zones[rapl->nb-1] = rapl_zones[zone];
	rapl->packages[rapl->nb-1] = package;
      }
#ifdef DEBUG
      printf("%d %d %d %d %s\n\n", length, package, zone, rapl_zones[zone], _name2);
#endif
    }
  }
#ifdef DEBUG
  printf("Result of init\n");
  for(int i=0; i<rapl->nb; i++)
    printf("package %d, zone %d, name %s\n", rapl->packages[i], rapl->zones[i], rapl->names[i]);
#endif

  rapl->values = calloc(sizeof(uint64_t), rapl->nb);
  rapl->tmp_values = calloc(sizeof(uint64_t), rapl->nb);

  _get_rapl(rapl->values, rapl);

  *ptr = (void*)rapl;
  return rapl->nb;
}



unsigned int get_rapl(uint64_t* results, void* ptr) {
  _rapl_t* state = (_rapl_t*) ptr;
  _get_rapl(state->tmp_values, state);
  for(int i=0; i<state->nb; i++)
    results[i] = state->tmp_values[i] - state->values[i];

  memcpy(state->values, state->tmp_values, sizeof(uint64_t)*state->nb);
  return state->nb;
}




void clean_rapl(void* ptr) {
  _rapl_t* rapl = (_rapl_t*) ptr;
  for (int package = 0; package < rapl->nb_pkgs; package++)
    if (powercap_rapl_destroy(&rapl->pkgs[package]))
      perror("powercap_rapl_destroy");
  for (int elem=0; elem<rapl->nb; elem++) 
      free(rapl->names[elem]);
  
  free(rapl->names);
  free(rapl->pkgs);
  free(rapl->zones);
  free(rapl->packages);
  free(rapl->values);
  free(rapl->tmp_values);
  free(rapl);
}

void label_rapl(char **labels, void *ptr) {
  _rapl_t* rapl = (_rapl_t*) ptr;
  for(int i=0; i<rapl->nb; i++)
    labels[i] = rapl->names[i];
}
*/
