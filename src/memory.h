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

unsigned int init_memory(char *, void **);
unsigned int get_memory(uint64_t *results, void *);
void clean_memory(void *);
void label_memory(char **labels, void *);


Sensor memory = {
    .init = init_memory,
    .get = get_memory,
    .clean = clean_memory,
    .label = label_memory,
    .nb_opt = 1,
};

Optparse memory_opt[1] = {
    {
        .longname = "memory",
        .shortname = 'm',
        .argtype = OPTPARSE_NONE,
        .usage_arg = NULL,
        .usage_msg = "Retrieves information about the memory via the syscall 'sysinfo(2)'.",
    },
};
