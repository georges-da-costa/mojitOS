// SPDX-License-Identifier: GPL-3.0-or-later

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "util.h"
#include <dirent.h>
#include <sys/stat.h>

#define BUFFER_SIZE 512

struct Temperature {
    char **label_list;
    int *fid_list;
    int nb_elem;
};
typedef struct Temperature Temperature;

int get_string(char *filename, char *buffer, int max_size)
{
    int fid = open(filename, O_RDONLY);

    //printf("Tries to open : %s : %d\n", filename, fid);
    if (fid == -1) {
        return -1;
    }

    int nb = read(fid, buffer, max_size);
    if (nb == -1) {
        close(fid);
        return -1;
    }

    buffer[nb] = 0;
    close(fid);
    return 0;
}

void add_to_list(char ***list_name, char *source, int nb_elem)
{
    //printf("Adds: %s\n", source);
    *list_name = (char**) realloc(*list_name, (nb_elem + 1) * sizeof(char *));
    (*list_name)[nb_elem] = (char*) malloc(strlen(source) + 1);
    strcpy((*list_name)[nb_elem], source);

}

void add_temperature_sensor(int id_rep, const char*name, Temperature *state)
{
    static int key = 0;
    static char buffer_filename[BUFFER_SIZE];
    static char buffer_label[BUFFER_SIZE];

    int delta = snprintf(buffer_label, BUFFER_SIZE, "Temp_%d_%s_", key, name);


    unsigned long sensor_max = 0;
    DIR *dir;
    struct dirent *ent;

    snprintf(buffer_filename, BUFFER_SIZE, "/sys/class/hwmon/hwmon%d/", id_rep);

    if ((dir = opendir (buffer_filename)) != NULL) {
	while ((ent = readdir (dir)) != NULL) {
	    if( strncmp(ent->d_name, "temp", 4) == 0) {
		unsigned int suffix_l = strlen("input");
		if (strlen(ent->d_name) > suffix_l) {
		    if (strncmp(ent->d_name+strlen(ent->d_name)-suffix_l, "input", suffix_l) == 0) {
			unsigned long current_val = strtoul(ent->d_name+strlen("temp"), NULL, 10);
			if ( current_val > sensor_max)
			    sensor_max = current_val;
		    }
		}
	    }
	}
	closedir (dir);
    } else {
	return;
    }

    for (unsigned int i = 1; i<=sensor_max; i++) {

        snprintf(buffer_filename, BUFFER_SIZE, "/sys/class/hwmon/hwmon%d/temp%d_input", id_rep, i);
        int fd = open(buffer_filename, O_RDONLY);
        if (fd < 0) {
	    continue;
        }
        state->fid_list = (int*) realloc(state->fid_list, (state->nb_elem + 1) * sizeof(int));
        state->fid_list[state->nb_elem] = fd;


        snprintf(buffer_filename, BUFFER_SIZE, "/sys/class/hwmon/hwmon%d/temp%d_label", id_rep, i);

        if (get_string(buffer_filename, buffer_label + delta, 100) == -1) {
	    snprintf(buffer_label+delta, BUFFER_SIZE, "%d", i);
        }

        for (unsigned int pos = 0; pos < strlen(buffer_label); pos++) {
            if (buffer_label[pos] == ' ') {
                buffer_label[pos] = '_';
            }

            if (buffer_label[pos] == '\n') {
                buffer_label[pos] = '\0';
            }
        }

        add_to_list(&state->label_list, buffer_label, state->nb_elem);

        state->nb_elem++;
        // printf("%s : %s\n", buffer_label, buffer_filename);
    }

    key++;
}

unsigned int init_temperature(char *args, void **ptr)
{
    UNUSED(args);
    Temperature *state = (Temperature *) malloc(sizeof(Temperature));
    state->nb_elem = 0;
    state->label_list = NULL;
    state->fid_list = NULL;

    char dir_name[] = "/sys/class/hwmon/hwmon%d/";
    char base_name[] = "/sys/class/hwmon/hwmon%d/name";
    static char name[BUFFER_SIZE];
    static char buffer[BUFFER_SIZE];

    int i = 0;
    snprintf(name, BUFFER_SIZE, dir_name, i);
    struct stat sb;
    while (stat(name, &sb) == 0 && S_ISDIR(sb.st_mode)) {
	snprintf(name, BUFFER_SIZE, base_name, i);

	if (get_string(name, buffer, 100) != -1) {
	    buffer[strlen(buffer)-1]='\0';
	    add_temperature_sensor(i, buffer, state);
	}

        i++;
        snprintf(name, BUFFER_SIZE, dir_name, i);
    }

    *ptr = (void *) state;
    return state->nb_elem;
}

unsigned int get_temperature(uint64_t *results, void *ptr)
{
    Temperature *state = (Temperature *)ptr;
    static char buffer[BUFFER_SIZE];

    for (int i = 0; i < state->nb_elem; i++) {
        if (pread(state->fid_list[i], buffer, 100, 0) < 0) {
            perror("pread");
            exit(1);
        }
        results[i] = strtoull(buffer, NULL, 10);
    }

    return state->nb_elem;
}

void clean_temperature(void *ptr)
{
    Temperature *state = (Temperature *)ptr;

    for (int i = 0; i < state->nb_elem; i++) {
        free(state->label_list[i]);
        close(state->fid_list[i]);
    }

    free(state->label_list);
    free(state->fid_list);
    free(state);
}

void label_temperature(const char **labels, void *ptr)
{
    Temperature *state = (Temperature *)ptr;

    for (int i = 0; i < state->nb_elem; i++) {
        labels[i] = state->label_list[i];
    }
}
