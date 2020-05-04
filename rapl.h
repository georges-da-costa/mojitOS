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

#include <powercap/powercap-rapl.h>

struct _rapl_t {
  powercap_rapl_pkg* pkgs;
  uint32_t nb_pkgs;
  
  uint32_t nb;

  char **names;
  uint32_t* zones;
  uint32_t* packages;
};

typedef struct _rapl_t _rapl_t;

_rapl_t* init_rapl(const uint32_t nb_zones, const int *rapl_zones);
void get_rapl(uint64_t *values, _rapl_t* rapl);
void clean_rapl(_rapl_t* rapl);
