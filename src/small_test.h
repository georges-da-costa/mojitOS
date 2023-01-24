#ifndef __SMALL_TEST_H
#define __SMALL_TEST_H 

#define FMT_NULL(string) \
	string = string ? string : "NULL"

#define TEST_STR(result, expected) \
	test_str(__FILE__, __LINE__, result, expected)

#define TEST_BOOLEAN(result, expected) \
	test_boolean(__FILE__, __LINE__, result, expected)

#define TEST_PTR(result, expected) \
	test_ptr(__FILE__, __LINE__, result, expected)

#define TEST_UINT64_T(result, expected) \
	test_uint64_t(__FILE__, __LINE__, result, expected)

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
    sprintf(buffer, "%ld", *value);
    return buffer;
}

void test(char *file, int line, void *result, void *expected, Comparator *compare, Formatter *format)
{
    static char buffer_result[1000];
    static char buffer_expected[1000];
    if (compare(result, expected) == 0) {
        printf("Test %s:%d failed: expected %s, got %s\n",
               file,
               line,
               format(buffer_result, expected),
               format(buffer_expected, result)
              );
    } else {
        printf("Test %s:%d passed\n", file, line);
    }
}

void test_str(char *file, int line, char *result, char *expected)
{
    Comparator *compare = (Comparator *) string_compare;
    Formatter *format = (Formatter *) string_format;

    test(file, line, result, expected, compare, format);
}

void test_boolean(char *file, int line, bool *result, bool *expected)
{
    Comparator *compare = (Comparator *) boolean_compare;
    Formatter *format = (Formatter *) boolean_format;

    test(file, line, (void *) result, (void *) expected, compare, format);
}

void test_ptr(char *file, int line, void *result, void *expected)
{
    Comparator *compare = (Comparator *) ptr_compare;
    Formatter *format = (Formatter *) ptr_format;

    test(file, line, result, expected, compare, format);
}

void test_uint64_t(char *file, int line, void *result, void *expected)
{
    Comparator *compare = (Comparator *) uint64_t_compare;
    Formatter *format = (Formatter *) uint64_t_format;

    test(file, line, (uint64_t *)result, (uint64_t *)expected, compare, format);
}
#endif
