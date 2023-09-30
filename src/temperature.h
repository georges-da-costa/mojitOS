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

unsigned int init_temperature(char *, void **);
unsigned int get_temperature(uint64_t *results, void *);
void clean_temperature(void *);
void label_temperature(char **labels, void *);

Sensor temperature = {
    .init = init_temperature,
    .get = get_temperature,
    .clean = clean_temperature,
    .label = label_temperature,
    .nb_opt = 1,
};

Optparse temperature_opt[1] = {
    {
        .longname = "cpu-temp",
        .shortname = 'c',
        .argtype = OPTPARSE_NONE,
        .usage_arg = NULL,
        .usage_msg = "processor temperature"
    },
};
