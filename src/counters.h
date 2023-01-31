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

unsigned int init_counters(char *, void **);
unsigned int get_counters(uint64_t *results, void *);
void clean_counters(void *);
void label_counters(char **labels, void *);
void *show_all_counters(void *, size_t);

Sensor counters = {
    .init = init_counters,
    .get = get_counters,
    .clean = clean_counters,
    .label = label_counters,
    .nb_opt = 2,
};

Optparse counters_opt[2] = {
    {
        .longname = "perf-list",
        .shortname = 'p',
        .argtype = OPTPARSE_REQUIRED,
        .usage_arg = "<perf_list>",
        .usage_msg = "performance counters\n"
        "\tperf_list is a coma separated list of performance counters.\n"
        "\tEx: instructions,cache_misses",
    },
    {
        .longname = "list",
        .shortname = 'l',
        .argtype = OPTPARSE_NONE,
        .usage_arg = NULL,
        .usage_msg = "list the available performance counters and quit",
        .fn = show_all_counters,
    },
};

