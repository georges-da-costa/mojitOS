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

unsigned int init_disk(char *, void **);
unsigned int get_disk(uint64_t *results, void *);
void clean_disk(void *);
void label_disk(const char **labels, void *);

Sensor disk = {
    .init = init_disk,
    .get = get_disk,
    .clean = clean_disk,
    .label = label_disk,
    .nb_opt = 1,
};

Optparse disk_opt[1] = {
    {
        .longname = "disk",
        .shortname = 'a',
        .argtype = OPTPARSE_NONE,
        .usage_arg = NULL,
        .usage_msg = "system disk/storage",
	.fn = NULL,
    },
};
