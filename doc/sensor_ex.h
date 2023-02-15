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
 * Example of a basic sensor: an accumulator
**/

unsigned int init_acc(char *, void **);
unsigned int get_acc(uint64_t *results, void *);
void clean_acc(void *);
void label_acc(char **labels, void *);

Sensor sensor_ex = {
    .init = init_acc,
    .get = get_acc,
    .clean = clean_acc,
    .label = label_acc,
    .nb_opt = 1,
};

Optparse sensor_ex_opt[1] = {
    {
        .longname = "accumulator",
        .shortname = 'a',
        .argtype = OPTPARSE_NONE,		/* OPTPARSE_NONE / OPTPARSE_OPTIONAL / OPTPARSE_REQUIRED */
        .usage_arg = NULL,
        .usage_msg = "dumb accumulator",
    },
};
