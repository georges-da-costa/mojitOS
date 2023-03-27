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

#include "small_test.h"
#include "../src/memory.c"

// In order to verify the integrity.
TFUNCTION(test_labels, {
    // If it fails update the tests
    int tested_count = 10;
    int expected_count = MEMORY_COUNT;
    TEST_INT(&tested_count, &expected_count);

    const char *result = NULL;
    char *expected = NULL;

    expected = "totalram";
    result =  memory_labels[TOTALRAM];
    TEST_STR(result, expected);

    expected = "freeram";
    result = memory_labels[FREERAM];
    TEST_STR(result, expected);

    expected = "sharedram";
    result = memory_labels[SHAREDRAM];
    TEST_STR(result, expected);

    expected = "bufferram";
    result = memory_labels[BUFFERRAM];
    TEST_STR(result, expected);

    expected = "totalswap";
    result = memory_labels[TOTALSWAP];
    TEST_STR(result, expected);

    expected = "freeswap";
    result = memory_labels[FREESWAP];
    TEST_STR(result, expected);

    expected = "procs";
    result = memory_labels[PROCS];
    TEST_STR(result, expected);

    expected = "totalhigh";
    result = memory_labels[TOTALHIGH];
    TEST_STR(result, expected);

    expected = "freehigh";
    result = memory_labels[FREEHIGH];
    TEST_STR(result, expected);

    expected = "mem_unit";
    result = memory_labels[MEM_UNIT];
    TEST_STR(result, expected);
})

TFILE_ENTRY_POINT(test_memory, {
    CALL_TFUNCTION(test_labels);
})

