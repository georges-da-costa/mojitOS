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

/*
 * compilation:
 *

# normally, this part is done with `configure.sh`, but here we need to
# do this manually
cat <<EOF > src/sensors.h
#include "sensor_ex.h"

#define NB_SENSOR 1
#define NB_SENSOR_OPT 1

void init_sensors(Optparse *opts, Sensor *sensors, size_t len, size_t offset, int *nb_defined)
{
    int opt_idx = offset;
    for (int i = 0; i < sensor_ex.nb_opt; i++) {
        opts[opt_idx++] = sensor_ex_opt[i];
    }
    sensors[(*nb_defined)++] = sensor_ex;
    assert((offset + *nb_defined) <= len);
}
EOF

# idem
echo 'CAPTOR_OBJ = doc/sensor_ex.o' > sensors.mk

# actual compilation here
gcc -std=gnu99 -Wall -Wpedantic -I./lib -I./doc -I./src -g -Og -c doc/sensor_ex.c -o obj/sensor_ex.o
gcc -std=gnu99 -Wall -Wpedantic -I./lib -I./doc -I./src -g -Og -c doc/util.c -o obj/util.o
gcc -std=gnu99 -Wall -Wpedantic -I./lib -I./doc -I./src -g -Og -c doc/mojitos.c -o obj/mojitos.o
gcc -std=gnu99 -Wall -Wpedantic -I./lib -I./doc -I./src -g -Og obj/util.o obj/mojitos.o obj/sensor_ex.o -o bin/mojitos

 *
**/

#include <stdint.h>
#include <stdlib.h>

#include "util.h"

#define NB_SENSOR 3

struct Accumulator {
    int v[NB_SENSOR];
};
typedef struct Accumulator Accumulator;

void _get_acc(int v[NB_SENSOR])
{
    for (int i = 0; i < NB_SENSOR; i++) {
        v[i]++;
    }
}

unsigned int init_acc(char *none, void **ptr)
{
    /* "none" refers to an optionnal command-line argument */
    /* there is none in this case, so this parameter is not used */
    UNUSED(none);

    Accumulator *state = malloc(sizeof(Accumulator));

    for (int i = 0; i < NB_SENSOR; i++) {
        state->v[i] = -1;
    }

    *ptr = (void *)state;
    _get_acc(state->v);

    return NB_SENSOR;
}

unsigned int get_acc(uint64_t *results, void *ptr)
{
    Accumulator *state = (Accumulator *)ptr;

    _get_acc(state->v);

    for (int i = 0; i < NB_SENSOR; i++) {
        results[i] = state->v[i];
    }

    return NB_SENSOR;
}

char *_labels_accumulator[NB_SENSOR] = {"acc1", "acc2", "acc3"};
void label_acc(char **labels, void *none)
{
    UNUSED(none);
    for (int i = 0; i < NB_SENSOR; i++) {
        labels[i] = _labels_accumulator[i];
    }
}

void clean_acc(void *ptr)
{
    Accumulator *state = (Accumulator *)ptr;

    if (state == NULL) {
        return;
    }

    free(state);
}
