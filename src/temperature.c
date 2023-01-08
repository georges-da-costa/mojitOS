/*******************************************************
 Copyright (C) 2018-2021 Georges Da Costa <georges.da-costa@irit.fr>

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
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>

struct temperature_t
{
    char **label_list;
    int *fid_list;
    int nb_elem;
};

int get_string(char *filename, char *buffer, int max_size)
{
    int fid = open(filename, O_RDONLY);
    //printf("Tries to open : %s : %d\n", filename, fid);
    if(fid == -1)
        return -1;

    int nb = read(fid, buffer, max_size);
    if(nb == -1)
        {
            close(fid);
            return -1;
        }

    buffer[nb]=0;
    close(fid);
    return 0;
}

void add_to_list(char ***list_name, char *source, int nb_elem)
{
    //printf("Adds: %s\n", source);
    *list_name = realloc(*list_name, (nb_elem+1)*sizeof(char *));
    (*list_name)[nb_elem] = malloc(strlen(source)+1);
    strcpy((*list_name)[nb_elem], source);

}

void add_temperature_sensor(int id_rep, struct temperature_t *state)
{
    static int key=0;
    static char buffer_filename[512];
    static char buffer_label[512];

    int delta = sprintf(buffer_label, "Temp_%d_", key);

    for(int i=1;; i++)
        {
            sprintf(buffer_filename, "/sys/class/hwmon/hwmon%d/temp%d_label", id_rep, i);
            if(get_string(buffer_filename, buffer_label+delta, 100) == -1)
                break;

            for(int pos = 0; pos < strlen(buffer_label); pos++)
                {
                    if (buffer_label[pos] == ' ')  buffer_label[pos] = '_';
                    if (buffer_label[pos] == '\n') buffer_label[pos] = '\0';
                }
            add_to_list(&state->label_list, buffer_label, state->nb_elem);

            sprintf(buffer_filename, "/sys/class/hwmon/hwmon%d/temp%d_input", id_rep, i);
            state->fid_list = realloc(state->fid_list, (state->nb_elem+1)*sizeof(int));
            int fd = open(buffer_filename, O_RDONLY);
            if (fd < 0)
                {
                    fprintf(stderr, "%s ", buffer_filename);
                    perror("open");
                    exit(1);
                }
            state->fid_list[state->nb_elem] = fd;
            state->nb_elem++;
            // printf("%s : %s\n", buffer_label, buffer_filename);
        }

    key++;
}

unsigned int init_temperature(char *args, void **ptr)
{
    struct temperature_t *state = malloc(sizeof(struct temperature_t));
    state->nb_elem = 0;
    state->label_list = NULL;
    state->fid_list = NULL;

    char base_name[] = "/sys/class/hwmon/hwmon%d/name";
    static char name[512];
    static char buffer[512];

    int i = 0;
    sprintf(name, base_name, i);
    while(get_string(name, buffer, 8) != -1)
        {
            if (strcmp(buffer, "coretemp")==0)
                add_temperature_sensor(i, state);

            i++;
            sprintf(name, base_name, i);
        }
    *ptr = (void *) state;
    return state->nb_elem;
}

unsigned int get_temperature(uint64_t *results, void *ptr)
{
    struct temperature_t *state = (struct temperature_t *)ptr;
    static char buffer[512];
    for(int i=0; i<state->nb_elem; i++)
        {
            if (pread(state->fid_list[i], buffer, 100, 0) < 0)
                {
                    perror("pread");
                    exit(1);
                }
            results[i] = strtoull(buffer, NULL, 10);
        }
    return state->nb_elem;
}

void clean_temperature(void *ptr)
{
    struct temperature_t *state = (struct temperature_t *)ptr;

    for(int i=0; i<state->nb_elem; i++)
        {
            free(state->label_list[i]);
            close(state->fid_list[i]);
        }
    free(state->label_list);
    free(state->fid_list);
    free(state);
}

void label_temperature(char **labels, void *ptr)
{
    struct temperature_t *state = (struct temperature_t *)ptr;
    for(int i=0; i<state->nb_elem; i++)
        labels[i] = state->label_list[i];
}
