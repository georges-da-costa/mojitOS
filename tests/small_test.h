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

#ifndef __SMALL_TEST_H
#define __SMALL_TEST_H

#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "../src/util.h"

// ---------------------------API_INTERFACE
/**
 * @brief Define the entry point of the tests
 * It initializes each useful variables.
 */

#define TMAIN(code)                                              \
  int main()                                                     \
{                                                                \
  unsigned int __indentation_level = 0;                          \
  INDENTED_PRINT("%s:%s\n", __FILE__, __func__);                 \
  unsigned int __error_counter__ = 0;                            \
  do code while (0);                                             \
  DEFERRED_FILE_ERROR(__error_counter__);                        \
  return __error_counter__;                                      \
}

/**
 * @brief Define the entry point of a test file.
 * This macro is used to define the entry point of a test file.
 * It defines a function with the specified __filename that contains the test code specified in code.
 *
 * When the function is called, it initializes the test file using the INIT_TEST_FILE macro,
 * declares an integer variable __error_counter__ to keep track of any errors encountered during the tests,
 * executes the test code in a do-while loop, and then checks for any deferred errors using the DEFERRED_ERROR macro.
 * The function returns the value of __error_counter__,
 * which indicates the number of errors encountered during the tests.
 *
 * @param __filename The name of the function that serves as the entry point for the test file.
 * @param __code The test code to be executed in the function.
 */
#define TFILE_ENTRY_POINT(__filename, __code)       \
  int __filename (unsigned int __indentation_level) \
{                                                   \
  INIT_TEST_FILE();                                 \
  int __error_counter__ = 0;                        \
  do __code while(0);                               \
  DEFERRED_FILE_ERROR(__error_counter__);           \
  return __error_counter__;                         \
}

/**
 * @brief Define a test function within a test file.
 * This macro is used to define a test function within a test file.
 * It defines a function with the specified function_name that contains the test code specified in code.
 *
 * When the function is called, it initializes the test function using the INIT_TEST_FUNCTION macro,
 * declares an integer variable __error_counter__ to keep track of any errors encountered during the tests,
 * executes the test code in a do-while loop, and then checks for any deferred errors using the DEFERRED_ERROR macro.
 * The function returns the value of __error_counter__, which indicates the number of errors encountered during the tests.
 *
 * @param __function_name The name of the test function.
 * @param __code The test code to be executed in the function.
 */
#define TFUNCTION(__function_name, __code) \
  int __function_name(unsigned int __indentation_level) \
{ \
  INIT_TEST_FUNCTION(); \
  int __error_counter__ = 0; \
  do __code while(0); \
  DEFERRED_FUNCTION_ERROR(__error_counter__); \
  return __error_counter__; \
}

/**
 * @brief Call a test function within a test file.
 * This macro is used to call a test function within a test file.
 * It calls the function specified by function_name and adds the return value to the __error_counter__ variable.
 * This allows multiple test functions to be executed and their error count to be accumulated.
 *
 * @param function_name The name of the test function to be called.
 */
#define CALL_TFUNCTION(function_name) \
  do {__error_counter__ += function_name(__indentation_level + 1);} while(0)

/**
 * @def TEST_STR(result, expected)
 * @brief Test strings
 * This macro is used to test strings. It takes two arguments: `__result`, which is the string to be tested, and `__expected`, which is the expected value of the string.
 * The macro uses the `test_str()` function to perform the test, and provides the `__FILE__`, `__LINE__` preprocessor macros to indicate the location of the test in the source code.
 *
 * @param __result the string to be tested
 * @param __expected the expected value of the string
 *
 * @code
 * TEST_STR("Hello", "Hello");
 * @endcode
 */
#define TEST_STR(__result, __expected) \
    do {__error_counter__ += test(__FILE__, __LINE__, __indentation_level, __result, __expected, &str_interface);} while(0)

/**
 * @def TEST_BOOL(__result, __expected)
 * @brief Test bools
 * This macro is used to test bools. It takes two arguments: `__result`, which is the bool to be tested, and `__expected`, which is the expected value of the bool.
 * The macro uses the `test_bool()` function to perform the test, and provides the `__FILE__`, `__LINE__` preprocessor macros to indicate the location of the test in the source code.
 *
 * @param __result the bool to be tested
 * @param __expected the expected value of the bool
 *
 * @code
 * TEST_BOOL(1 == 1, true);
 * @endcode
 */
#define TEST_BOOL(__result, __expected) \
    do {__error_counter__ += test(__FILE__, __LINE__, __indentation_level, __result, __expected, &bool_interface);} while (0)

/**
 * @def TEST_PTR(__result, __expected)
 * @brief Test pointers
 * This macro is used to test pointers. It takes two arguments: `__result`, which is the pointer to be tested, and `__expected`, which is the expected value of the pointer.
 * The macro uses the `test_ptr()` function to perform the test, and provides the `__FILE__`, `__LINE__` preprocessor macros to indicate the location of the test in the source code.
 *
 * @param __result the pointer to be tested
 * @param __expected the expected value of the pointer
 *
 * @code
 * int x = 5;
 * int *ptr = &x;
 * TEST_PTR(ptr, &x);
 * @endcode
 */
#define TEST_PTR(__result, __expected) \
	do {__error_counter__ += test(__FILE__, __LINE__, __indentation_level, __result, __expected, &ptr_interface);} while(0)


/**
 * @def TEST_UINT64_T(__result, __expected)
 * @brief Test 64-bit unsigned integers
 * This macro is used to test 64-bit unsigned integers. It takes two arguments: `__result`, which is the integer to be tested, and `__expected`, which is the expected value of the integer.
 * The macro uses the `test_uint64_t()` function to perform the test, and provides the `__FILE__`, `__LINE__` preprocessor macros to indicate the location of the test in the source code.
 *
 * @param __result the integer to be tested
 * @param __expected the expected value of the integer
 *
 * @code
 * TEST_UINT64_T(5, 5);
 * @endcode
 */
#define TEST_UINT64_T(__result, __expected) \
	do {__error_counter__ += test(__FILE__, __LINE__, __indentation_level, __result, __expected, &u64_interface);} while(0)

#define TEST_INTERFACE(__result, __expected, __interface) \
	do {__error_counter__ += test(__FILE__, __LINE__, __indentation_level, __result, __expected, __interface);} while(0)


/**
 * @def TEST_T_ARRAY(function, nb_error, size, results, expecteds)
 * @brief Test arrays of data
 * The macro uses a for loop to iterate through the array and apply the test function to each element.
 *
 * @param __test_macro the test function to be used on each element of the array
 * @param __array_size the number of elements in the array
 * @param __results the array of elements to be tested
 * @param __expecteds the array of expected values

 * @code
 * int results[3] = {1, 2, 3};
 * int expecteds[3] = {1, 2, 3};
 * TEST_T_ARRAY(TEST_INT, 3, results, expecteds);
 * @endcode
*/
#define TEST_T_ARRAY(__test_macro, __array_size, __results, __expecteds)  \
	for (unsigned int i = 0; i < __array_size; i++) {                       \
      __test_macro(__results[i], __expecteds[i]);                         \
	}


// --------------------------------API_CODE
// These functions should not be used, only use the previous macros.


#define INDENTED_PRINT(__fmt, ...)                          \
  do {                                                      \
    for(unsigned int i = 0; i < __indentation_level; i++) { \
      printf("|    ");                                       \
    }                                                       \
    printf(__fmt, ##__VA_ARGS__);                           \
  } while(0)


#define INIT_TEST_FILE()     \
  INDENTED_PRINT("%s:%s\n", __FILE__, __func__)

#define INIT_TEST_FUNCTION() \
  INDENTED_PRINT("%s()\n", __func__);

#define DEFERRED_FILE_ERROR(nb_error) \
    INDENTED_PRINT("|_Deferred Error : %u\n",nb_error);

#define DEFERRED_FUNCTION_ERROR(nb_error) \
    INDENTED_PRINT("|_Deferred Error : %d\n",nb_error);

typedef int (Comparator) (void *, void *);
typedef char *(Formatter) (char *, void *);

typedef struct {
    Comparator *compare;
    Formatter *format;
} TestInterface;

//  ---------------------------str_interface

int str_compare(void *ptr1, void *ptr2)
{
    char *str1 = (char *) ptr1;
    char *str2 = (char *) ptr2;

    if (str1 == NULL && str2 == NULL) {
        return 1;
    } else if (str1 == NULL || str2 == NULL) {
        return 0;
    } else {
        return (strcmp(str1, str2) == 0);
    }
}

char *str_format(char *buffer, void *ptr)
{
    UNUSED(buffer);
    static char *str_null = "NULL";
    char *str = (char *) ptr;
    return str ? str : str_null;
}

static const TestInterface str_interface = {
    .compare = str_compare,
    .format = str_format
};

//  --------------------------bool_interface


int bool_compare(void *ptr1, void *ptr2)
{
    bool *bool1 = (bool *) ptr1;
    bool *bool2 = (bool *) ptr2;
    return *bool1 == *bool2;
}

char *bool_format(char *buffer, void *ptr)
{
    UNUSED(buffer);
    bool *_bool = (bool *) ptr;
    return *_bool ? "True" : "False";
}

static const TestInterface bool_interface = {
    .compare = bool_compare,
    .format = bool_format
};

// ---------------------------ptr_interface

int ptr_compare(void *ptr1, void *ptr2)
{
    return ptr1 == ptr2;
}

char *ptr_format(char *buffer, void *ptr)
{
    sprintf(buffer, "%p", ptr);
    return buffer;
}

static const TestInterface ptr_interface = {
    .compare = ptr_compare,
    .format = ptr_format
};

// ---------------------------u64_interface

int u64_compare(void *ptr1, void *ptr2)
{
    uint64_t *v1 = (uint64_t *) ptr1;
    uint64_t *v2 = (uint64_t *) ptr2;
    return *v1 == *v2;
}

char *u64_format(char *buffer, void *ptr)
{
    uint64_t *v = (uint64_t *) ptr;
    sprintf(buffer, "%"PRIu64"", *v);
    return buffer;
}

static const TestInterface u64_interface = {
    .compare = u64_compare,
    .format = u64_format
};

// ---------------------------test_function

int test(char *file, int line, unsigned int __indentation_level, void *result, void *expected, const TestInterface *interface)
{
    __indentation_level += 1;
    static char buffer_result[1000];
    static char buffer_expected[1000];
    int is_equal = interface->compare(result, expected);

    char *fmt_result = interface->format(buffer_expected, expected);
    char *fmt_expected = interface->format(buffer_result, result);
    if  (!is_equal) {
        INDENTED_PRINT("%s:%d: failed, expected <%s>, got <%s>\n", file, line, fmt_expected, fmt_result);
    }
    return !is_equal;
}

#endif

