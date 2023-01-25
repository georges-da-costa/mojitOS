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

#include "util.h"

/**
 * @brief Substracts lhs by rhs, assuming that lhs is a cyclic increment from rhs, 
 * meaning that if lhs is greater, rhs's value overflowed.
 * @param lhs 
 * @param rhs 
 * @return uint64_t 
 */
uint64_t modulo_substraction(const uint64_t lhs, const uint64_t rhs)
{
    return rhs >= lhs ? (rhs - lhs)
           : (UINT64_MAX - lhs + 1) + rhs;
}
