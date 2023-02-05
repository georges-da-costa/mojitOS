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

unsigned int init_amd_rapl(char *, void **);
unsigned int get_amd_rapl(uint64_t *results, void *);
void clean_amd_rapl(void *);
void label_amd_rapl(char **labels, void *);

struct optparse_long amd_rapl_opt = {"amd_rapl", 'a', OPTPARSE_NONE};
struct captor amd_rapl = {
    .usage_arg = NULL,
    .usage_msg = "AMD_RAPL",
    .init = init_amd_rapl,
    .get = get_amd_rapl,
    .clean = clean_amd_rapl,
    .label = label_amd_rapl,
};
