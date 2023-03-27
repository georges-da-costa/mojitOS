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
#include "./../lib/info_reader.h"

TFUNCTION(test_replace_first, {
    // useful variables :
    char result[100];
    char expected[100];

    // -- Setup
    strcpy(result, "This is my string");
    strcpy(expected, "ThIs is my string");
    // -- Run
    replace_first(result, 'i', 'I');
    // -- Verification
    TEST_STR(result, expected);

    // -- Setup
    strcpy(result, "This is my string");
    strcpy(expected, "This is my string");
    // -- Run
    replace_first(result, 'x', 'X');
    // -- Verification
    TEST_STR(result, expected);

    // -- Setup
    strcpy(result, "This is my string");
    strcpy(expected, "This_is my string");
    // -- Run
    replace_first(result, ' ', '_');
    // -- Verification
    TEST_STR(result, expected);

    // -- Setup
    strcpy(result, "This is my string");
    strcpy(expected, "This_is my string");
    // -- Run
    replace_first(result, ' ', '_');
    // -- Verification
    TEST_STR(result, expected);

    // -- Setup
    strcpy(result, "This is my string");
    strcpy(expected, "This is my string");
    // -- Run
    replace_first(result, 'T', 'T');
    // -- Verification
    TEST_STR(result, expected);
})

TFUNCTION(test_split_on_delimiter, {
    // Useful variables
    char string[100];
    char *result_key;
    char *result_value;
    char expected_key[100];
    char expected_value[100];

    // Setup
    strcpy(string, "key:value");
    strcpy(expected_key, "key");
    strcpy(expected_value, "value");
    // Run
    split_on_delimiter(string, ":", &result_key, &result_value);
    // Verification
    TEST_STR(result_key, expected_key);
    TEST_STR(result_value, expected_value);

    // Setup
    strcpy(string, "key: value");
    strcpy(expected_key, "key");
    strcpy(expected_value, " value");
    // Run
    split_on_delimiter(string, ":", &result_key, &result_value);
    // Verification
    TEST_STR(result_key, expected_key);
    TEST_STR(result_value, expected_value);

    // Setup
    strcpy(string, "key:value");
    strcpy(expected_key, "key");
    strcpy(expected_value, "value");
    replace_first(string, ':', ' ');
    // Run
    split_on_delimiter(string, " ", &result_key, &result_value);
    // Verification
    TEST_STR(result_key, expected_key);
    TEST_STR(result_value, expected_value);

    // Setup
    strcpy(string, "");
    // Run
    split_on_delimiter(string, ":", &result_key, &result_value);
    // Verification
    TEST_STR(result_key, NULL);
    TEST_STR(result_value, NULL);

    // Setup
    strcpy(string, "key:value:extra");
    strcpy(expected_key, "key");
    strcpy(expected_value, "value:extra");
    // Run
    split_on_delimiter(string, ":", &result_key, &result_value);
    // Verification
    TEST_STR(result_key, expected_key);
    TEST_STR(result_value, expected_value);

    // Setup
    strcpy(string, "key: value :extra");
    strcpy(expected_key, "key");
    strcpy(expected_value, " value :extra");
    // Run
    split_on_delimiter(string, ":", &result_key, &result_value);
    // Verification
    TEST_STR(result_key, expected_key);
    TEST_STR(result_value, expected_value);
})

TFUNCTION(test_start_with, {
    char *prefix = NULL;
    char *string = NULL;
    bool result = false;
    bool _true = true;
    bool _false = false;

    prefix = "Hello";
    string = "Hello World";
    result = start_with(prefix, string);
    TEST_BOOL(&result, &_true);

    prefix = "Goodbye";
    string = "Hello World";
    result = start_with(prefix, string);
    TEST_BOOL(&result, &_false);

    prefix = "Hello World";
    string = "Hello";
    result = start_with(prefix, string);
    TEST_BOOL(&result, &_false);

    prefix = "Hello";
    string = "Hello";
    result = start_with(prefix, string);
    TEST_BOOL(&result, &_true);

    prefix = NULL;
    string = "Hello World";
    result = start_with(prefix, string);
    TEST_BOOL(&result, &_false);
})

#define NONE 0
#define INIT_KEYFINDER(__key_finder, __key, __delimiter, __copy, __set)  \
  do {                                                                   \
    __key_finder = (KeyFinder) {                                         \
      .key = __key,                                                      \
      .delimiter = __delimiter,                                          \
      .copy = __copy,                                                    \
      .set = __set                                                       \
    };                                                                   \
  } while (0);

#define DUMMY_KEYFINDER(__key_finder, __key, __delimiter) \
  INIT_KEYFINDER(__key_finder, __key, __delimiter, NONE, NONE)    \

#define INIT_PARSER(__parser, __storage, __nb_stored, __capacity,     \
                    __storage_struct_size, __keys, __nb_keys, __file) \
do {                                                                  \
    __parser = (Parser) {                                             \
      .storage = __storage,                                           \
      .nb_stored = __nb_stored,                                       \
      .capacity = __capacity,                                         \
      .storage_struct_size = __storage_struct_size,                   \
      .keys = __keys,                                                 \
      .nb_keys = __nb_keys,                                           \
      .file = __file                                                  \
    };                                                                \
  } while (0);


#define DUMMY_PARSER(__parser, __keys, __nb_keys) \
  INIT_PARSER(__parser, NONE, NONE, NONE, NONE, __keys, __nb_keys, NONE)


TFUNCTION(test_match, {
    // useful variables :
    bool _true = true;
    bool _false = false;
    KeyFinder keys[10];
    char line[100];
    Parser parser;
    bool result;
    KeyFinder *found_key_finder = NULL;
    char *raw_value = NULL;

    // Test 1:
    // -- Setup
    DUMMY_KEYFINDER(keys[0], "key", ": ");
    DUMMY_PARSER(parser, keys, 1);
    strcpy(line, "key: value");
    found_key_finder = NULL;
    raw_value = NULL;
    // -- Run
    result = match(&parser, line, &found_key_finder, &raw_value);
    // -- Verification
    TEST_BOOL(&result, &_true);
    TEST_PTR(found_key_finder, &keys[0]);
    TEST_STR(raw_value, "value");

    // Test 2:
    // -- Setup
    DUMMY_KEYFINDER(keys[0], "key", ": ");
    DUMMY_PARSER(parser, keys, 1)
    strcpy(line, "not a key: value");
    found_key_finder = NULL;
    raw_value = NULL;
    // -- Run
    result = match(&parser, line, &found_key_finder, &raw_value);
    // -- Verification
    TEST_BOOL(&result, &_false);
    TEST_PTR(found_key_finder, NULL);
    TEST_STR(raw_value, NULL);

    // Test 3:
    // -- Setup
    DUMMY_KEYFINDER(keys[0],"key", ": ");
    DUMMY_PARSER(parser, keys, 1);
    strcpy(line, "key:value");
    found_key_finder = NULL;
    raw_value = NULL;
    // -- Run
    result = match(&parser, line, &found_key_finder, &raw_value);
    // -- Verification
    TEST_BOOL(&result, &_false);
    TEST_PTR(found_key_finder, NULL);
    TEST_STR(raw_value, NULL);

    // Test 4:
    // -- Setup
    DUMMY_KEYFINDER(keys[0], "key", ": ");
    DUMMY_KEYFINDER(keys[1], "second_key", ": ");
    DUMMY_PARSER(parser, keys, 2);
    strcpy(line, "second_key: value");
    found_key_finder = NULL;
    raw_value = NULL;
    // -- Run
    result = match(&parser, line, &found_key_finder, &raw_value);
    // -- Verification
    TEST_BOOL(&result, &_true);
    TEST_PTR(found_key_finder, &keys[1]);
    TEST_STR(raw_value, "value");

    // Test 5:
    // -- Setup
    DUMMY_KEYFINDER(keys[0], "key", ": ");
    DUMMY_PARSER(parser, keys, 1);
    strcpy(line, "");
    found_key_finder = NULL;
    raw_value = NULL;
    // -- Run
    result = match(&parser, line, &found_key_finder, &raw_value);
    TEST_BOOL(&result, &_false);
    TEST_PTR(found_key_finder, NULL);
    TEST_STR(raw_value, NULL);
})


#define __NB_KEYS 4

typedef struct {
  int values[__NB_KEYS];
} IntArray;

GenericPointer __test_file_int_allocator(char *s)
{
    unsigned int value = atoi(s);
    return (GenericPointer) value;
}

void __test_file_set_int(GenericPointer storage, GenericPointer data)
{
    IntArray *array = (IntArray*) storage;
    int i = (int) data;
    array->values[i] = i;
}

TFUNCTION(test_dummy_file, {
    KeyFinder keys[__NB_KEYS];
    INIT_KEYFINDER(keys[0], "int0", " : ", __test_file_int_allocator, __test_file_set_int);
    INIT_KEYFINDER(keys[1], "int1", " ", __test_file_int_allocator, __test_file_set_int);
    INIT_KEYFINDER(keys[2], "int2", " -> ", __test_file_int_allocator, __test_file_set_int);
    INIT_KEYFINDER(keys[3], "int3", "--", __test_file_int_allocator, __test_file_set_int);

    IntArray results;
    IntArray expected;

    expected.values[0] = 0;
    expected.values[1] = 1;
    expected.values[2] = 2;
    expected.values[3] = 3;

    Parser parser;
    FILE* file = fopen("./tests/info_reader_test.txt", "r");
    INIT_PARSER(parser, (GenericPointer) &results, 0, 1, sizeof(IntArray), keys, __NB_KEYS, file);
    parse(&parser);

    for (unsigned int i = 0; i < __NB_KEYS; i++) {
        TEST_INT(&(results.values[i]), &(expected.values[i]));
    }
})

TFILE_ENTRY_POINT(test_info_reader, {
    CALL_TFUNCTION(test_replace_first);
    CALL_TFUNCTION(test_split_on_delimiter);
    CALL_TFUNCTION(test_start_with);
    CALL_TFUNCTION(test_match);
    CALL_TFUNCTION(test_dummy_file);
})

