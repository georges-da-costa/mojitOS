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

unsigned int init_network(char *, void **);
unsigned int get_network(uint64_t *results, void *);
void clean_network(void *);
void label_network(char **labels, void *);

struct optparse_long network_opt = {"net-dev", 'd', OPTPARSE_REQUIRED};
struct captor network = {
    .usage_arg = "<net_dev>",
    .usage_msg = "network monitoring (if network_device is X, tries to detect it automatically)",
    .init = init_network,
    .get = get_network,
    .clean = clean_network,
    .label = label_network,
};

