#include "small_test.h"

TFUNCTION(test_replace_first, {
    char test1[] = "This is my string";
    replace_first(test1, 'i', 'I');
    TEST_STR(test1, "ThIs is my string");

    char test2[] = "This is my string";
    replace_first(test2, 'x', 'X');
    TEST_STR(test2, "This is my string");


    char test3[] = "This is my string";
    replace_first(test3, ' ', '_');
    TEST_STR(test3, "This_is my string");
})

TFUNCTION(test_split_on_delimiter, {
    char test4[] = "key:value";
    char *key;
    char *value;

    split_on_delimiter(test4, ":", &key, &value);
    TEST_STR(key, "key");
    TEST_STR(value, "value");

    char test5[] = "key: value";
    split_on_delimiter(test5, ":", &key, &value);
    TEST_STR(key, "key");
    TEST_STR(value, " value");

    char test6[] = "key:value";
    replace_first(test6, ':', ' ');
    split_on_delimiter(test6, " ", &key, &value);
    TEST_STR(key, "key");
    TEST_STR(value, "value");

    char test7[] = "";
    split_on_delimiter(test7, ":", &key, &value);
    TEST_STR(key, NULL);
    TEST_STR(value, NULL);

    char test9[] = "key:value:extra";
    split_on_delimiter(test9, ":", &key, &value);
    TEST_STR(key, "key");
    TEST_STR(value, "value:extra");

    char test10[] = "key: value :extra";
    split_on_delimiter(test10, ":", &key, &value);
    TEST_STR(key, "key");
    TEST_STR(value, " value :extra");
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
    TEST_BOOLEAN(&result, &_true);

    prefix = "Goodbye";
    string = "Hello World";
    result = start_with(prefix, string);
    TEST_BOOLEAN(&result, &_false);

    prefix = "Hello World";
    string = "Hello";
    result = start_with(prefix, string);
    TEST_BOOLEAN(&result, &_false);

    prefix = "Hello";
    string = "Hello";
    result = start_with(prefix, string);
    TEST_BOOLEAN(&result, &_true);

    prefix = NULL;
    string = "Hello World";
    result = start_with(prefix, string);
    TEST_BOOLEAN(&result, &_false);
})

#define DUMB_KEYFINDER(key_finder, key, delimiter)  \
	do { 											\
		key_finder = (KeyFinder) {					\
			key,									\
			delimiter,								\
			0,										\
			0										\
		}; 											\
	} while (0);

#define DUMB_PARSER(parser, keys, nb_keys)  \
	do {									\
		parser = (Parser) {					\
			0,			   					\
			0,			   					\
			0,			   					\
			0,  							\
			keys,			   				\
			nb_keys, 		   				\
			0								\
		};							   		\
	} while (0);


TFUNCTION(test_match, {
    // usefull variable :
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
    DUMB_KEYFINDER(keys[0], "key", ": ");
    DUMB_PARSER(parser, keys, 1);
    strcpy(line, "key: value");
    found_key_finder = NULL;
    raw_value = NULL;
    // -- Run
    result = match(&parser, line, &found_key_finder, &raw_value);
    // -- Verification
    TEST_BOOLEAN(&result, &_true);
    TEST_PTR(found_key_finder, &keys[0]);
    TEST_STR(raw_value, "value");

    // Test 2:
    // -- Setup
    DUMB_KEYFINDER(keys[0], "key", ": ");
    DUMB_PARSER(parser, keys, 1)
    strcpy(line, "not a key: value");
    found_key_finder = NULL;
    raw_value = NULL;
    // -- Run
    result = match(&parser, line, &found_key_finder, &raw_value);
    // -- Verification
    TEST_BOOLEAN(&result, &_false);
    TEST_PTR(found_key_finder, NULL);
    TEST_STR(raw_value, NULL);

    // Test 3:
    // -- Setup
    DUMB_KEYFINDER(keys[0],"key", ": ");
    DUMB_PARSER(parser, keys, 1);
    strcpy(line, "key:value");
    found_key_finder = NULL;
    raw_value = NULL;
    // -- Run
    result = match(&parser, line, &found_key_finder, &raw_value);
    // -- Verification
    TEST_BOOLEAN(&result, &_false);
    TEST_PTR(found_key_finder, NULL);
    TEST_STR(raw_value, NULL);

    // Test 4:
    // -- Setup
    DUMB_KEYFINDER(keys[0], "key", ": ");
    DUMB_KEYFINDER(keys[1], "second_key", ": ");
    DUMB_PARSER(parser, keys, 2);
    strcpy(line, "second_key: value");
    found_key_finder = NULL;
    raw_value = NULL;
    // -- Run
    result = match(&parser, line, &found_key_finder, &raw_value);
    // -- Verification
    TEST_BOOLEAN(&result, &_true);
    TEST_PTR(found_key_finder, &keys[1]);
    TEST_STR(raw_value, "value");

    // Test 5:
    // -- Setup
    DUMB_KEYFINDER(keys[0], "key", ": ");
    DUMB_PARSER(parser, keys, 1);
    strcpy(line, "");
    found_key_finder = NULL;
    raw_value = NULL;
    // -- Run
    result = match(&parser, line, &found_key_finder, &raw_value);
    TEST_BOOLEAN(&result, &_false);
    TEST_PTR(found_key_finder, NULL);
    TEST_STR(raw_value, NULL);
})

TFILE_ENTRY_POINT(test_info_reader, {
    CALL_TFUNCTION(test_replace_first);
    CALL_TFUNCTION(test_split_on_delimiter);
    CALL_TFUNCTION(test_start_with);
    CALL_TFUNCTION(test_match);
})

#ifdef __TESTING_INFO_READER__
int main()
{
    test_info_reader();
    return 0;
}
#endif

