/*******************************************************
 Copyright (C) 2022-2023 Georges Da Costa <georges.da-costa@irit.fr>

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

unsigned int init_rapl(char *, void **);
unsigned int get_rapl(uint64_t *results, void *);
void clean_rapl(void *);
void label_rapl(char **labels, void *);

Sensor rapl = {
    .init = init_rapl,
    .get = get_rapl,
    .clean = clean_rapl,
    .label = label_rapl,
    .nb_opt = 1,
};

Optparse rapl_opt[1] = {
    {
        .longname = "intel-rapl",
        .shortname = 'r',
        .argtype = OPTPARSE_NONE,
        .usage_arg = NULL,
        .usage_msg = "INTEL RAPL",
    },
};

