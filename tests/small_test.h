#ifndef __SMALL_TEST_H
#define __SMALL_TEST_H

#include <stdbool.h>
#include <string.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>


// ---------------------------API_INTERFACE

#define INIT_TEST_FILE() \
	init_test_file(__FILE__, __func__)

#define INIT_TEST_FUNCTION() \
	init_test_function(__func__)

#define DEFERRED_ERROR(nb_error) \
	printf("========== Deferred Error : %d\n", nb_error)

#define TEST_STR(result, expected) \
	test_str(__FILE__, __LINE__, result, expected)

#define TEST_BOOLEAN(result, expected) \
	test_boolean(__FILE__, __LINE__, result, expected)

#define TEST_PTR(result, expected) \
	test_ptr(__FILE__, __LINE__, result, expected)

#define TEST_UINT64_T(result, expected) \
	test_uint64_t(__FILE__, __LINE__, result, expected)

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

