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

unsigned int init_memory_counters(char *args, void **ptr);
unsigned int get_memory_counters(uint64_t *results, void *ptr);
void label_memory_counters(char **labels, void *ptr);
void clean_memory_counters(void *ptr);
void *show_all_memory_counters(void *, size_t);

Sensor memory_counters = {
    .init = init_memory_counters,
    .get = get_memory_counters,
    .clean = clean_memory_counters,
    .label = label_memory_counters,
    .nb_opt = 2,
};

Optparse memory_counters_opt[2] = {
    {
        .longname = "memory-counters",
        .shortname = 'M',
        .argtype = OPTPARSE_REQUIRED,
        .usage_arg = "<memory_list>",
        .usage_msg =
            "memory counters\n"
            "\tmemory_list is a coma separated list of memory counters.\n"
            "\tEx: Zswap,Zswapped",
    },
    {
        .longname = "memory-list",
        .shortname = 'L',
        .argtype = OPTPARSE_NONE,
        .usage_arg = NULL,
        .usage_msg = "list the available memory counters and quit",
        .fn = show_all_memory_counters,
    },
};
