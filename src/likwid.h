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

unsigned int init_likwid(char *, void **);
unsigned int get_likwid(uint64_t *results, void *);
void clean_likwid(void *);
void label_likwid(char **labels, void *);
void *show_all_likwid(void *, size_t);

Sensor likwid = {
    .init = init_likwid,
    .get = get_likwid,
    .clean = clean_likwid,
    .label = label_likwid,
    .nb_opt = 2,
};

Optparse likwid_opt[2] = {
    {
        .longname = "likwid",
        .shortname = 'k',
        .argtype = OPTPARSE_REQUIRED,
        .usage_arg = "<perf_list>",
        .usage_msg = "performance counters\n"
        "\tperf_list is a coma separated list of performance counters with associated register.\n"
        "\tEx: FP_ARITH_INST_RETIRED_128B_PACKED_DOUBLE:PMC0,FP_ARITH_INST_RETIRED_SCALAR_DOUBLE:PMC1",
    },
    {
        .longname = "list-likwid",
        .shortname = 'w',
        .argtype = OPTPARSE_NONE,
        .usage_arg = NULL,
        .usage_msg = "list the available performance counters and quit",
        .fn = show_all_likwid,
    },
};

