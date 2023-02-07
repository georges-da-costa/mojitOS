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

struct accumulator_t {
    int v[NB_SENSOR];
};

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

    struct accumulator_t *state = malloc(sizeof(struct accumulator_t));

    for (int i = 0; i < NB_SENSOR; i++) {
        state->v[i] = -1;
    }

    *ptr = (void *)state;
    _get_acc(state->v);

    return NB_SENSOR;
}

unsigned int get_acc(uint64_t *results, void *ptr)
{
    struct accumulator_t *state = (struct accumulator_t *)ptr;

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
    struct accumulator_t *state = (struct accumulator_t *)ptr;

    if (state == NULL) {
        return;
    }

    free(state);
}
