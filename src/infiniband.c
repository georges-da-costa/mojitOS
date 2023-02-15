/*******************************************************
 Copyright (C) 2018-2023 Georges Da Costa <georges.da-costa@irit.fr>

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
#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <glob.h>
#include <stdint.h>
#include "util.h"

#define NB_SENSOR 4

struct Network {
    uint64_t values[NB_SENSOR];
    uint64_t tmp_values[NB_SENSOR];
    int sources[NB_SENSOR];
};
typedef struct Network Network;

unsigned int _get_network(uint64_t *results, int *sources);


unsigned int init_infiniband(char *infi_path, void **ptr)
{
    if (infi_path == NULL) {
        return 0;
    }

    if (strcmp(infi_path, "X") == 0) {

        glob_t res;

        glob("/sys/class/infiniband/*/ports/*/counters/", 0, NULL, &res);

        if (res.gl_pathc == 0) {
            return 0;
        }

        infi_path = res.gl_pathv[0];
    }

    char *filenames[] = {"%s/port_rcv_packets",
                         "%s/port_rcv_data",
                         "%s/port_xmit_packets",
                         "%s/port_xmit_data"
                        };

    Network *state = malloc(sizeof(Network));

    char buffer[1024];
    for (int i = 0; i < NB_SENSOR; i++) {
        snprintf(buffer, 1024, filenames[i], infi_path);
        state->sources[i] = open(buffer, O_RDONLY);
    }

    *ptr = (void *) state;
    _get_network(state->values, state->sources);

    return NB_SENSOR;
}

char *_labels_infiniband[NB_SENSOR] = {"irxp", "irxb", "itxp", "itxb"};
void label_infiniband(char **labels, void *none)
{
    UNUSED(none);

    for (int i = 0; i < NB_SENSOR; i++) {
        labels[i] = _labels_infiniband[i];
    }
}
