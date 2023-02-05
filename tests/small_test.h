#ifndef __SMALL_TEST_H
#define __SMALL_TEST_H

#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>

#include "../src/util.h"

// ---------------------------API_INTERFACE
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
  __error_counter__ += function_name(__indentation_level + 1)


/**
 * @def TEST_STR(result, expected)
 * @brief Test strings
 * This macro is used to test strings. It takes two arguments: `result`, which is the string to be tested, and `expected`, which is the expected value of the string.
 * The macro uses the `test_str()` function to perform the test, and provides the `__FILE__`, `__LINE__` preprocessor macros to indicate the location of the test in the source code.
 *
 * @param result the string to be tested
 * @param expected the expected value of the string
 *
 * @code
 * TEST_STR("Hello", "Hello");
 * @endcode
 */
#define TEST_STR(result, expected) \
    __error_counter__ += test_str(__FILE__, __LINE__, __indentation_level, result, expected)

/**
 * @def TEST_BOOLEAN(result, expected)
 * @brief Test booleans
 * This macro is used to test booleans. It takes two arguments: `result`, which is the boolean to be tested, and `expected`, which is the expected value of the boolean.
 * The macro uses the `test_boolean()` function to perform the test, and provides the `__FILE__`, `__LINE__` preprocessor macros to indicate the location of the test in the source code.
 *
 * @param result the boolean to be tested
 * @param expected the expected value of the boolean
 *
 * @code
 * TEST_BOOLEAN(1 == 1, true);
 * @endcode
 */
#define TEST_BOOLEAN(result, expected) \
	__error_counter__ += test_boolean(__FILE__, __LINE__, __indentation_level, result, expected)

/**
 * @def TEST_PTR(result, expected)
 * @brief Test pointers
 * This macro is used to test pointers. It takes two arguments: `result`, which is the pointer to be tested, and `expected`, which is the expected value of the pointer.
 * The macro uses the `test_ptr()` function to perform the test, and provides the `__FILE__`, `__LINE__` preprocessor macros to indicate the location of the test in the source code.
 *
 * @param result the pointer to be tested
 * @param expected the expected value of the pointer
 *
 * @code
 * int x = 5;
 * int *ptr = &x;
 * TEST_PTR(ptr, &x);
 * @endcode
 */
#define TEST_PTR(result, expected) \
	__error_counter__ += test_ptr(__FILE__, __LINE__, __indentation_level, result, expected)


/**
 * @def TEST_UINT64_T(result, expected)
 * @brief Test 64-bit unsigned integers
 * This macro is used to test 64-bit unsigned integers. It takes two arguments: `result`, which is the integer to be tested, and `expected`, which is the expected value of the integer.
 * The macro uses the `test_uint64_t()` function to perform the test, and provides the `__FILE__`, `__LINE__` preprocessor macros to indicate the location of the test in the source code.
 *
 * @param result the integer to be tested
 * @param expected the expected value of the integer
 *
 * @code
 * TEST_UINT64_T(5, 5);
 * @endcode
 */
#define TEST_UINT64_T(result, expected) \
	__error_counter__ += test_uint64_t(__FILE__, __LINE__, __indentation_level, result, expected)

/**
 * @def TEST_T_ARRAY(function, nb_error, size, results, expecteds)
 * @brief Test arrays of data
 * The macro uses a for loop to iterate through the array and apply the test function to each element,
 * adding any errors to the nb_error variable.
 *
 * @param function the test function to be used on each element of the array
 * @param nb_error the number of errors encountered during the test
 * @param size the number of elements in the array
 * @param results the array of elements to be tested
 * @param expecteds the array of expected values

 * @code
 * int results[3] = {1, 2, 3};
 * int expecteds[3] = {1, 2, 3};
 * TEST_T_ARRAY(TEST_INT, errors, 3, results, expecteds);
 * @endcode
*/
#define TEST_T_ARRAY(function, size, results, expecteds)	\
	for (unsigned int i = 0; i < size; i++) {						\
		function(results[i], expecteds[i]);				\
	}


// --------------------------------API_CODE


#define INDENTED_PRINT(__fmt, ...)                          \
  do {                                                      \
    for(unsigned int i = 0; i < __indentation_level; i++) { \
      printf("|    ");                                       \
    }                                                       \
    printf(__fmt, ##__VA_ARGS__);                           \
  } while(0)

/**
 * @def INIT_TEST_FILE()
 * @brief Initialize the test file
 * This macro is used to initialize the test file. It takes the `__FILE__` and `__func__` preprocessor macros as arguments, which provide the name of the current file and the current function, respectively.
 *
 * @param __FILE__ preprocessor macro that provides the name of the current file
 * @param __func__ preprocessor macro that provides the name of the current function
 *
 * @code
 * INIT_TEST_FILE();
 * @endcode
 */
#define INIT_TEST_FILE()     \
  INDENTED_PRINT("%s:%s\n", __FILE__, __func__)

/**
 * @def INIT_TEST_FUNCTION()
 * @brief Initialize the test function
 * This macro is used to initialize the test function. It takes the `__func__` preprocessor macro as an argument, which provides the name of the current function.
 *
 * @param __func__ preprocessor macro that provides the name of the current function
 *
 * @code
 * INIT_TEST_FUNCTION();
 * @endcode
 */
#define INIT_TEST_FUNCTION() \
  INDENTED_PRINT("%s()\n", __func__);

#define DEFERRED_FILE_ERROR(nb_error) \
    INDENTED_PRINT("|_Deferred Error : %u\n",nb_error);
//INDENTED_PRINT("Deferred Error in %s: %d\n",__FILE__, nb_error);

#define DEFERRED_FUNCTION_ERROR(nb_error) \
    INDENTED_PRINT("|_Deferred Error : %d\n",nb_error);

#define FMT_NULL(string) \
	string = string ? string : "NULL"

typedef int (Comparator) (void *, void *);
typedef char *(Formatter) (char *, void *);

int string_compare(char *string1, char *string2)
{
    if (string1 == NULL && string2 == NULL) {
        return 1;
    } else if (string1 == NULL || string2 == NULL) {
        return 0;
    } else {
        return (strcmp(string1, string2) == 0);
    }
}

char *string_format(char *buffer, char *string)
{
  UNUSED(buffer); 
  return FMT_NULL(string);
}


int boolean_compare(bool *boolean1, bool *boolean2)
{
    return *boolean1 == *boolean2;
}

char *boolean_format(char *buffer, bool *boolean)
{
    UNUSED(buffer);
    return *boolean ? "True" : "False";
}

int ptr_compare(void *ptr1, void *ptr2)
{
    return ptr1 == ptr2;
}

char *ptr_format(char *buffer, void *ptr)
{
    sprintf(buffer, "%p", ptr);
    return buffer;
}


int uint64_t_compare(uint64_t *value1, uint64_t *value2)
{
    return *value1 == *value2;
}

char *uint64_t_format(char *buffer, uint64_t *value)
{
    sprintf(buffer, "%"PRIu64"", *value);
    return buffer;
}

int test(char *file, int line, unsigned int __indentation_level, void *result, void *expected, Comparator *compare, Formatter *format)
{
    __indentation_level++;
    static char buffer_result[1000];
    static char buffer_expected[1000];
    int is_equal = compare(result, expected);

    char *fmt_result = format(buffer_result, expected);
    char *fmt_expected = format(buffer_expected, result);
    if  (!is_equal) {
        INDENTED_PRINT("%s:%d: failed, expected <%s>, got <%s>\n", file, line, fmt_expected, fmt_result);
    }
    return !is_equal;
}

int test_str(char *file, int line,unsigned int __indentation_level, char *result, char *expected)
{
    Comparator *compare = (Comparator *) string_compare;
    Formatter *format = (Formatter *) string_format;

    return test(file, line, __indentation_level, result, expected, compare, format);
}

int test_boolean(char *file, int line, unsigned int __indentation_level, bool *result, bool *expected)
{
    Comparator *compare = (Comparator *) boolean_compare;
    Formatter *format = (Formatter *) boolean_format;

    return test(file, line, __indentation_level, (int *) result, (void *) expected, compare, format);
}

int test_ptr(char *file, int line, unsigned int __indentation_level, void *result, void *expected)
{
    Comparator *compare = (Comparator *) ptr_compare;
    Formatter *format = (Formatter *) ptr_format;

    return test(file, line, __indentation_level, result, expected, compare, format);
}

int test_uint64_t(char *file, int line, unsigned int __indentation_level, void *result, void *expected)
{
    Comparator *compare = (Comparator *) uint64_t_compare;
    Formatter *format = (Formatter *) uint64_t_format;

    return test(file, line, __indentation_level, (uint64_t *)result, (uint64_t *)expected, compare, format);
}
#endif

