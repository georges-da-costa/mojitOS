/*******************************************************
 Copyright (C) 2023-2023 Georges Da Costa <georges.da-costa@irit.fr>

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

unsigned int init_nvidia_sensor(char *, void **);
unsigned int get_nvidia_sensor(uint64_t *results, void *);
void clean_nvidia_sensor(void *);
void label_nvidia_sensor(char **labels, void *);


Sensor nvidia_gpu = {
    .init = init_nvidia_sensor,
    .get = get_nvidia_sensor,
    .clean = clean_nvidia_sensor,
    .label = label_nvidia_sensor,
    .nb_opt = 1,
};

Optparse nvidia_gpu_opt[1] = {
    {
        .longname = "nvidia",
        .shortname = 'n',
        .argtype = OPTPARSE_NONE,
        .usage_arg = NULL,
        .usage_msg = "NVIDIA GPU",
    },
};
