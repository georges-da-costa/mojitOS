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

// ~/mojitos/doc$ gcc -Wall -Wextra -Wpedantic -o test_main_ex test_main_ex.c ./../src/util.c
// Include of the test library
#include "./../tests/small_test.h"

// Include the *.c files containing the tests
#include "./test_file_ex.c"
// #include "./test_another_test_file.c"

// Define the entry point of the programme
TMAIN({
    // Must contain the call to the entry point functions of each file
    // USE CALL_TFUNCTION(my_function) instead of my_function()
    CALL_TFUNCTION(test_file_ex);
    // CALL_TFUNCTION(another_file_entry_point_function);
})

