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
#include <stdint.h>
#include "util.h"

#define NB_SENSOR 4

static char *route = "/proc/net/route";
char *_labels_network[NB_SENSOR] = {
    "%s:rxp",
    "%s:rxb",
    "%s:txp",
    "%s:txb",
};
struct network_t {
    uint64_t values[NB_SENSOR];
    uint64_t tmp_values[NB_SENSOR];
    int sources[NB_SENSOR];
    char labels[NB_SENSOR][128];
};

unsigned int _get_network(uint64_t *results, int *sources)
{
    if (sources == NULL) {
        return 0;
    }

    char buffer[128];

    for (int i = 0; i < NB_SENSOR; i++) {
        if (pread(sources[i], buffer, 127, 0) < 0) {
            perror("pread");
            exit(1);
        }

        results[i] = strtoull(buffer, NULL, 10);
    }

    return NB_SENSOR;
}

unsigned int init_network(char *dev, void **ptr)
{
    if (dev == NULL) {
        return 0;
    }

    if (strcmp(dev, "X") == 0) {
        int fd = open(route, O_RDONLY);

        if (fd < 0) {
            fprintf(stderr, "%s ", route);
            perror("open");
            exit(1);
        }

        char buffer[1000];

        if (read(fd, buffer, 999) < 0 ) {
            perror("read");
            close(fd);
            exit(1);
        }

        char *start_of_dev = strchr(buffer, '\n') + 1;
        char *end_of_dev = strchr(start_of_dev, '\t');
        *end_of_dev = '\0';
        dev = start_of_dev;
        close(fd);
    }

    char *filenames[] = {
        "/sys/class/net/%s/statistics/rx_packets",
        "/sys/class/net/%s/statistics/rx_bytes",
        "/sys/class/net/%s/statistics/tx_packets",
        "/sys/class/net/%s/statistics/tx_bytes",
    };

    struct network_t *state = malloc(sizeof(struct network_t));

    char buffer2[256];
    for (int i = 0; i < NB_SENSOR; i++) {
        snprintf(buffer2, sizeof(buffer2), filenames[i], dev);
        state->sources[i] = open(buffer2, O_RDONLY);
        snprintf(state->labels[i], sizeof(state->labels[i]), _labels_network[i], dev);
    }

    *ptr = (void *) state;
    _get_network(state->values, state->sources);

    return NB_SENSOR;
}

unsigned int get_network(uint64_t *results, void *ptr)
{
    struct network_t *state = (struct network_t *) ptr;
    _get_network(state->tmp_values, state->sources);

    for (int i = 0; i < NB_SENSOR; i++) {
        results[i] = modulo_substraction(state->tmp_values[i], state->values[i]);
    }

    memcpy(state->values, state->tmp_values, NB_SENSOR * sizeof(uint64_t));
    return NB_SENSOR;
}

void clean_network(void *ptr)
{
    struct network_t *state = (struct network_t *) ptr;

    if (state == NULL) {
        return;
    }

    for (int i = 0; i < NB_SENSOR; i++) {
        close(state->sources[i]);
    }

    free(state);
}

void label_network(char **labels, void *ptr)
{
    struct network_t *state = (struct network_t *) ptr;

    for (int i = 0; i < NB_SENSOR; i++) {
        labels[i] = state->labels[i];
    }
}
