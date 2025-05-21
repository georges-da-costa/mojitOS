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
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "util.h"

#define NB_MAX_DEV 8
#define NB_SENSOR 4

static const char *route = "/proc/net/route";
const char *_labels_network[NB_SENSOR] = {
    "%s:rxp",
    "%s:rxb",
    "%s:txp",
    "%s:txb",
};
struct Network {
    uint64_t values[NB_MAX_DEV][NB_SENSOR];
    uint64_t tmp_values[NB_MAX_DEV][NB_SENSOR];
    int sources[NB_MAX_DEV][NB_SENSOR];
    char labels[NB_MAX_DEV][NB_SENSOR][128];
    char devs[NB_MAX_DEV][128];
    int ndev;
};
typedef struct Network Network;

static void _get_network(uint64_t *results, int *sources)
{
    if (sources == NULL) {
        return;
    }

    char buffer[128];

    for (int i = 0; i < NB_SENSOR; i++) {
        if (pread(sources[i], buffer, 127, 0) < 0) {
            perror("pread");
            exit(1);
        }

        results[i] = strtoull(buffer, NULL, 10);
    }
}

/*
 * read from fd len chars and store them into buf
 * make *s points to the first occurence of c into buf
*/
static int strchr_refill(int fd, char *buf, int len, char **s, char c)
{
    *s = strchr(*s, c);

    if (*s == NULL) {
        int nbytes = read(fd, buf, len - 1);
        if (nbytes < 0) {
            perror("read");
            return -1;
        }
        buf[len - 1] = '\0';

        /* whole file read */
        if (nbytes == 0) {
            return 0;
        }

        *s = strchr(buf, c);
    }

    return 1;
}

unsigned int init_network(char *dev, void **ptr)
{
    if (dev == NULL) {
        exit(1);
    }

    const char *filenames[] = {
        "/sys/class/net/%s/statistics/rx_packets",
        "/sys/class/net/%s/statistics/rx_bytes",
        "/sys/class/net/%s/statistics/tx_packets",
        "/sys/class/net/%s/statistics/tx_bytes",
    };

    struct Network *state = (struct Network *) malloc(sizeof(struct Network));
    memset(state, '\0', sizeof(*state));

    if (strcmp(dev, "X") == 0) {
        int fd = open(route, O_RDONLY);

        if (fd < 0) {
            fprintf(stderr, "%s ", route);
            perror("open");
            exit(1);
        }

        char buffer[1000] = {0};

        /* skip first line */
        char *s = buffer;
        int ret = strchr_refill(fd, buffer, sizeof(buffer), &s, '\n');
        if (ret != 1) {
            close(fd);
            free(state);
            exit(1);
        }
        s++;

        char *start_of_dev = s;
        /* jump to the end of the device name */
        ret = strchr_refill(fd, buffer, sizeof(buffer), &s, '\t');
        if (ret != 1) {
            close(fd);
            free(state);
            exit(1);
        }

        state->ndev++;	// ndev should be equal to 1 at this point
        memcpy(&(state->devs[state->ndev - 1]), start_of_dev,
               MIN((size_t)(sizeof(state->devs[0]) - 1), (size_t)(s - start_of_dev)));

        for (;;) {
            /* jump to the next line */
            ret = strchr_refill(fd, buffer, sizeof(buffer), &s, '\n');
            if (ret != 1) {
                break;
            }
            s++;

            start_of_dev = s;
            ret = strchr_refill(fd, buffer, sizeof(buffer), &s, '\t');
            if (ret != 1) {
                break;
            }

            /* compare dev name to the previously saved one */
            int newdev = 1;
            for (int i = 0; i < state->ndev && newdev; i++) {
                if (strncmp(start_of_dev, state->devs[i], s - start_of_dev) == 0) {
                    newdev = 0;
                }
            }
            if (newdev) {
                if (state->ndev >= NB_MAX_DEV) {
                    fprintf(stderr, "Maximum amount of network devices exceeded (%d).\n", NB_MAX_DEV);
                    break;
                }
                state->ndev++;
                memcpy(&(state->devs[state->ndev - 1]), start_of_dev,
                       MIN((size_t)(sizeof(state->devs[0]) - 1), (size_t)(s - start_of_dev)));
            }
        }

        close(fd);
    } else {
        state->ndev = 1;
        memcpy(&(state->devs[0]), dev, strlen(dev) + 1);
    }

    char buffer2[256];
    for (int i = 0; i < state->ndev; i++) {
        for (int j = 0; j < NB_SENSOR; j++) {
            snprintf(buffer2, sizeof(buffer2), filenames[j], state->devs[i]);
            errno = 0;
            int fd = open(buffer2, O_RDONLY);
            if (fd < 0) {
                fprintf(stderr, "init_network: open: %s: %.*s\n", strerror(errno),
                        (int)sizeof(buffer2), buffer2);
                free(state);
                exit(1);
            }
            state->sources[i][j] = fd;
            snprintf(state->labels[i][j], sizeof(state->labels[i][j]), _labels_network[j],
                     state->devs[i]);
        }
    }

    *ptr = (void *) state;

    for (int i = 0; i < state->ndev; i++) {
        _get_network(state->values[i], state->sources[i]);
    }

    return state->ndev * NB_SENSOR;
}

unsigned int get_network(uint64_t *results, void *ptr)
{
    struct Network *state = (struct Network *) ptr;

    for (int i = 0; i < state->ndev; i++) {
        _get_network(state->tmp_values[i], state->sources[i]);

        for (int j = 0; j < NB_SENSOR; j++) {
            results[i*NB_SENSOR + j] = modulo_substraction(state->tmp_values[i][j], state->values[i][j]);
        }

        memcpy(&(state->values[i]), &(state->tmp_values[i]),
               NB_SENSOR * sizeof(state->values[i][0]));
    }

    return state->ndev * NB_SENSOR;
}

void clean_network(void *ptr)
{
    Network *state = (Network *) ptr;

    if (state == NULL) {
        return;
    }

    for (int i = 0; i < state->ndev; i++) {
        for (int j = 0; j < NB_SENSOR; j++) {
            close(state->sources[i][j]);
        }
    }

    free(state);
}

void label_network(const char **labels, void *ptr)
{
    struct Network *state = (struct Network *) ptr;

    for (int i = 0; i < state->ndev; i++) {
        for (int j = 0; j < NB_SENSOR; j++) {
            labels[i*NB_SENSOR + j] = state->labels[i][j];
        }
    }
}
