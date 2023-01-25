#ifndef __SMALL_TEST_H
#define __SMALL_TEST_H

#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>


// ---------------------------API_INTERFACE

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
#define INIT_TEST_FILE() \
	init_test_file(__FILE__, __func__)

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
	init_test_function(__func__)

/**
 * @def DEFERRED_ERROR(nb_error)
 * @brief Print deferred errors
 * This macro is used to print deferred errors. It takes a single argument, `nb_error`, which is the number of errors encountered during the test.
 *
 * @param nb_error the number of errors encountered during the test
 *
 * @code
 * DEFERRED_ERROR(5);
 * @endcode
 */
#define DEFERRED_ERROR(nb_error) \
	printf("========== Deferred Error : %d\n", nb_error)

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
	test_str(__FILE__, __LINE__, result, expected)

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
	test_boolean(__FILE__, __LINE__, result, expected)

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
	test_ptr(__FILE__, __LINE__, result, expected)


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
	test_uint64_t(__FILE__, __LINE__, result, expected)

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
#define TEST_T_ARRAY(function, nb_error, size, results, expecteds)	\
	for (unsigned int i = 0; i < size; i++) {						\
		nb_error += function(results[i], expecteds[i]);				\
	}


// --------------------------------API_CODE

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

char *string_format(__attribute__((unused)) char *buffer, char *string)
{
    return FMT_NULL(string);
}


int boolean_compare(bool *boolean1, bool *boolean2)
{
    return *boolean1 == *boolean2;
}

char *boolean_format(__attribute__((unused)) char *buffer, bool *boolean)
{
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

void init_test_file(const char *file, const char *function)
{
    printf("========== TEST in %s -> %s()\n", file, function);
}

void init_test_function(const char *function)
{
    printf("|=> %s()\n", function);
}

int test(char *file, int line, void *result, void *expected, Comparator *compare, Formatter *format)
{
    static char buffer_result[1000];
    static char buffer_expected[1000];
    int is_equal = compare(result, expected);
    char c_result = is_equal ? 'V' : 'X';
    printf("[%c]    | %s:%d: ", c_result, file, line);

    if  (!is_equal) {
        printf("failed, expected %s, got %s\n",
               format(buffer_result, expected),
               format(buffer_expected, result)
              );
    } else {
        printf("passed\n");
    }
    return !is_equal;
}

int test_str(char *file, int line, char *result, char *expected)
{
    Comparator *compare = (Comparator *) string_compare;
    Formatter *format = (Formatter *) string_format;

    return test(file, line, result, expected, compare, format);
}

int test_boolean(char *file, int line, bool *result, bool *expected)
{
    Comparator *compare = (Comparator *) boolean_compare;
    Formatter *format = (Formatter *) boolean_format;

    return test(file, line, (int *) result, (void *) expected, compare, format);
}

int test_ptr(char *file, int line, void *result, void *expected)
{
    Comparator *compare = (Comparator *) ptr_compare;
    Formatter *format = (Formatter *) ptr_format;

    return test(file, line, result, expected, compare, format);
}

int test_uint64_t(char *file, int line, void *result, void *expected)
{
    Comparator *compare = (Comparator *) uint64_t_compare;
    Formatter *format = (Formatter *) uint64_t_format;

    return test(file, line, (uint64_t *)result, (uint64_t *)expected, compare, format);
}
#endif

