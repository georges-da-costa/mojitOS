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

#include "util.c"
#include "amd_rapl.c"
#include "info_reader.c"
#include "memory.c"

TMAIN({
    CALL_TFUNCTION(test_util);
    CALL_TFUNCTION(test_amd_rapl);
    CALL_TFUNCTION(test_info_reader);
    CALL_TFUNCTION(test_memory);
})
