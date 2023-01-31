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

unsigned int init_infiniband(char *infi_path, void **ptr);
void label_infiniband(char **labels, void *);

Captor infiniband = {
    .init = init_infiniband,
    .get = NULL,
    .clean = NULL,
    .label = label_infiniband,
    .nb_opt = 1,
};

Optparse infiniband_opt[1] = {
    {
        .longname = "monitor-infiniband",
        .shortname = 'i',
        .argtype = OPTPARSE_REQUIRED,
        .usage_arg = "<infiniband_path>",
        .usage_msg = "infiniband monitoring (if infiniband_path is X, tries to detect it automatically)",
    },
};

