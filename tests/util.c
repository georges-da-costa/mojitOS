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

#include "../src/util.h"
#include "small_test.h"


TFUNCTION(test_modulo_substraction, {
    uint64_t lhs = 0;
    uint64_t rhs = 0;
    uint64_t result = 0;
    uint64_t expected = 0;

    // Test 1:
    // -- Setup
    lhs = 10;
    rhs = 10;
    expected = 0;
    // -- Run
    result = modulo_substraction(lhs, rhs);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // Test 2:
    // -- Setup
    lhs = UINT64_MAX;
    rhs = 0;
    expected = UINT64_MAX;
    // -- Run
    result = modulo_substraction(lhs, rhs);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // Test 3:
    // -- Setup
    lhs = 0;
    rhs = UINT64_MAX;
    expected = 1;
    // -- Run
    result = modulo_substraction(lhs, rhs);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // Test 4:
    // -- Setup
    lhs = 10;
    rhs = 20;
    expected = UINT64_MAX - 9;
    // -- Run
    result = modulo_substraction(lhs, rhs);
    // -- Verification
    TEST_UINT64_T(&result, &expected);

    // Test 5:
    // -- Setup
    lhs = 1000;
    rhs = 1000;
    expected = 0;
    result = modulo_substraction(lhs, rhs);
    TEST_UINT64_T(&result, &expected);
})

TFUNCTION(test_max, {
    int expected = 0;
    int result = 0;

    expected = 10;
    result = MAX(expected, 9);
    TEST_INT(&result, &expected);

    expected = -15;
    result = MAX(expected, -16);
    TEST_INT(&result, &expected);

    expected = 0;
    result = MAX(expected, -1);
    TEST_INT(&result, &expected);

    expected = 1;
    result = MAX(expected, 0);
    TEST_INT(&result, &expected);
})

TFUNCTION(test_min, {
    int expected = 0;
    int result = 0;

    expected = 9;
    result = MIN(expected, 10);
    TEST_INT(&result, &expected);

    expected = -16;
    result = MIN(expected, -15);
    TEST_INT(&result, &expected);

    expected = -1;
    result = MIN(expected, 0);
    TEST_INT(&result, &expected);

    expected = 0;
    result = MIN(expected, 1);
    TEST_INT(&result, &expected);
})

TFILE_ENTRY_POINT(test_util, {
    CALL_TFUNCTION(test_modulo_substraction);
    CALL_TFUNCTION(test_max);
    CALL_TFUNCTION(test_min);
})
