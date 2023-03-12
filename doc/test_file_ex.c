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
// Include the file that contains the function you want to test
// #include "a_file.c"

// This example is divided into three parts:
// 1. SOME SIMPLE EXAMPLES
// 2. A MORE COMPLEX EXAMPLE
// 3. FILE ENTRY POINT

//  -----------------1. SOME SIMPLE EXAMPLES

// A simple function that should pass
// test_should_pass is the name of the function
// {} is the function code
// Use one of the define TEST_XXX defined in small_test.h
TFUNCTION(test_should_pass, {
    int result = 42;
    int expected = 42;
    TEST_INT(&result, &expected);
})

// A simple function that should pass
// test_should_fail is the name of the function
// {} is the function code
// Use one of the define TEST_XXX defined in small_test.h
TFUNCTION(test_should_fail, {
    char *result = "a fail";
    char *expected = "a nice fail";
    TEST_STR(result, expected);
})

// A simple test on an array
// test_array is the name of the function
// {} is the function code
// Use one of the define TEST_XXX defined in small_test.h
TFUNCTION(test_array, {
    static unsigned int size = 10;
    int results[10] = {0};
    int expecteds[10] = {0};
    for (unsigned int i = 0; i < size; i++)
    {
        TEST_INT(&results[i], &expecteds[i]);
    }
})

// See "TFILE_ENTRY_POINT" for how to call these functions.

// ------------2. THE MORE COMPLEX EXAMPLE
// This example shows how to add a new type to the framework

// The type that you want to add :
typedef struct {
    int simple_int;
    char simple_str[20];
} UserType;


// The test framework needs two functions :

// A function that compares two values.
int usertype_compare(void *ptr1, void *ptr2)
{
    return memcmp(ptr1, ptr2, sizeof(UserType)) == 0;
}

// A function to format a value.
char *usertype_format(char buffer[FMT_BUFFER_SIZE], void *ptr)
{
    UserType *x = (UserType *) ptr;
    snprintf(buffer, FMT_BUFFER_SIZE, "UserType {simple_int: %d, simple_str: %s}", x->simple_int, x->simple_str);
    return buffer;
}

// Store the functions in the interface type.
static const TestInterface usertype_interface = {
    .compare = usertype_compare,
    .format = usertype_format
};

// Create a macro that implements the interface by passing the interface created to the TEST_INTERFACE macro.
#define TEST_USER_TYPE(__result, __expected) \
    TEST_INTERFACE(__result, __expected, &usertype_interface)


// Creating a constructor is recommended (C can have problems parsing macros where "," is used).
#define DUMMY_USER_TYPE(__user, __simple_int, __simple_str) \
    do {                                                    \
      __user = (UserType) {                                 \
          .simple_int = __simple_int,                       \
          .simple_str = __simple_str                        \
    };                                                      \
} while (0);

// Now you can write test for your new type
// all you have to do is call the macro that implements "TEST_INTERFACE".
// here it's "TEST_USERTYPE"
TFUNCTION(test_user_type, {
    UserType result;
    UserType expected;

    DUMMY_USER_TYPE(result, 1, "John Doe");
    DUMMY_USER_TYPE(expected, 1, "John Doe");

    TEST_USER_TYPE(&result, &expected);
})

// Now you can write test for your new type
// all you have to do is call the macro that implements "TEST_INTERFACE".
// here it's "TEST_USERTYPE"
TFUNCTION(test_array_user_type, {
    UserType results[1];
    UserType expecteds[1];

    DUMMY_USER_TYPE(results[0], 1, "John Doe");
    DUMMY_USER_TYPE(expecteds[0], 1, "John Doe");

    for (unsigned int i = 0; i < 1; i++)
    {
        TEST_USER_TYPE(&results[i], &expecteds[i]);
    }
})

// ---------------------3. FILE ENTRY POINT

// Define the entry point of a test file
// Use the macro "CALL_TFUNCTION" where the name of a function is defined with the macro "TFUNCTION".
TFILE_ENTRY_POINT(test_file_ex, {
    // Call the simple examples
    CALL_TFUNCTION(test_should_pass);
    CALL_TFUNCTION(test_should_fail);
    CALL_TFUNCTION(test_array);

    // Call the more complex example
    CALL_TFUNCTION(test_user_type);
    CALL_TFUNCTION(test_array_user_type);
})

