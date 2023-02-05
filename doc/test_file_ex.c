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

// Include of the test library
#include "./../tests/small_test.h"

// -- A simple function that should pass
TFUNCTION(test_should_pass, {
    char *result = "It's going to pass";
    char *expected = "It's going to pass";
    TEST_STR(result, expected);
})

// -- A simple function that should fail
TFUNCTION(test_should_fail, {
    char *result = "a fail";
    char *expected = "a nice fail";
    TEST_STR(result, expected);
})

// -- Add a new type in the test framework

typedef struct {
    int simple_int;
    char simple_str[20];
} UserType;


// -- Implemet the interface
int usertype_compare(void *ptr1, void *ptr2)
{
    return memcmp(ptr1, ptr2, sizeof(UserType)) == 0;
}

char *usertype_format(char buffer[1000], void *ptr)
{
    UserType *x = (UserType *) ptr;
    sprintf(buffer, "UserType {simple_int: %d, simple_str: %s}", x->simple_int, x->simple_str);
    return buffer;
}

// Create a variable which contains the functions
static const TestInterface usertype_interface = {
    .compare = usertype_compare,
    .format = usertype_format
};

// -- Create the test macro to call
#define TEST_USER_TYPE(__result, __expected) \
    TEST_INTERFACE(__result, __expected, &usertype_interface);

// -- Create a macro setter
#define DUMMY_USER_TYPE(__user, __simple_int, __simple_str) \
    do {                                                    \
      __user = (UserType) {                                 \
          .simple_int = __simple_int,                       \
          .simple_str = __simple_str                        \
    };                                                      \
} while (0);

// -- Compare two usertype
TFUNCTION(test_user_type, {
    UserType x1;
    UserType x2;

    DUMMY_USER_TYPE(x1, 1, "John Doe");
    DUMMY_USER_TYPE(x2, 1, "John Doe");

    TEST_USER_TYPE(&x1, &x2);
})


// -- Compare an array of usetype
TFUNCTION(test_array_user_type, {
    UserType *results[1];
    UserType *expecteds[1];

    UserType x1;
    UserType x2;
    DUMMY_USER_TYPE(x1, 1, "John Doe");
    DUMMY_USER_TYPE(x2, 1, "John Doe");

    results[0] = &x1;
    expecteds[0] = &x2;

    TEST_T_ARRAY(TEST_USER_TYPE, 1, (void **)results, (void **)expecteds);
})

// Define the entry point of a test file
TFILE_ENTRY_POINT(test_file_ex, {
    CALL_TFUNCTION(test_should_pass);
    CALL_TFUNCTION(test_should_fail);
    CALL_TFUNCTION(test_user_type);
    CALL_TFUNCTION(test_array_user_type);
})
