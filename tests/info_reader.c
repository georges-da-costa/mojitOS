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
#define DUMMY_KEYFINDER(__key_finder, __key, __delimiter) \
  do {                                                    \
    __key_finder = (KeyFinder) {                          \
      .key = __key,                                       \
      .delimiter = __delimiter,                           \
      .copy = NONE,                                       \
      .set = NONE                                         \
    };                                                    \
  } while (0);

#define DUMMY_PARSER(__parser, __keys, __nb_keys) \
  do {                                            \
    __parser = (Parser) {                         \
      .storage = NONE,                            \
      .nb_stored = NONE,                          \
      .capacity = NONE,                           \
      .storage_struct_size = NONE,                \
      .keys = __keys,                             \
      .nb_keys = __nb_keys,                       \
      .file = NONE                                \
    };                                            \
  } while (0);


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

TFILE_ENTRY_POINT(test_info_reader, {
    CALL_TFUNCTION(test_replace_first);
    CALL_TFUNCTION(test_split_on_delimiter);
    CALL_TFUNCTION(test_start_with);
    CALL_TFUNCTION(test_match);
})

