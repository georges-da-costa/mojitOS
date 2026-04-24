/*******************************************************
 Copyright (C) 2026 Georges Da Costa <georges.da-costa@irit.fr>

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

unsigned int init_papi(char *, void **);
unsigned int get_papi(uint64_t *results, void *);
void clean_papi(void *);
void label_papi(const char **labels, void *);
void *show_all_papi(void *, size_t);

Sensor papi = {
    .init = init_papi,
    .get = get_papi,
    .clean = clean_papi,
    .label = label_papi,
    .nb_opt = 2,
};

Optparse papi_opt[2] = {
    {
        .longname = "papi",
        .shortname = 'x',
        .argtype = OPTPARSE_REQUIRED,
        .usage_arg = "<papi_list>",
        .usage_msg = "papi performance counters\n"
        "\tpapi_list is a coma separated list of performance counters.\n"
        "\tEx: PAPI_FP_OPS,PAPI_REF_CYC",
	.fn = NULL,
    },
    {
        .longname = "papi-list",
        .shortname = 'X',
        .argtype = OPTPARSE_NONE,
        .usage_arg = NULL,
        .usage_msg = "list the available PAPI performance counters and quit",
        .fn = show_all_papi,
    },
};

