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

#include <stdlib.h>
#include "util.h"


uint64_t modulo_substraction(const uint64_t lhs, const uint64_t rhs)
{
    return lhs >= rhs ? (lhs - rhs)
           : (UINT64_MAX - rhs + 1) + lhs;
}

uint64_t modulo_substraction_bound(const uint64_t lhs, const uint64_t rhs, const uint64_t modulo)
{
    return lhs >= rhs ? (lhs - rhs)
           : (modulo - rhs + 1) + lhs;
}

char* read_int(char* ptr, uint64_t *val) {
    while (*ptr > '9' || *ptr < '0')
        ptr++;

    *val = 0;
    while (*ptr <= '9' && *ptr >= '0') {
	*val = *val * 10 + (*ptr - '0');
	ptr++;
    }
    return ptr;	    

}
