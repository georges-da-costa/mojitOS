#include "small_test.h"

int test_replace_first()
{
    INIT_TEST_FUNCTION();
    int nb_error = 0;

    char test1[] = "This is my string";
    replace_first(test1, 'i', 'I');
    nb_error += TEST_STR(test1, "ThIs is my string");

    char test2[] = "This is my string";
    replace_first(test2, 'x', 'X');
    nb_error += TEST_STR(test2, "This is my string");


    char test3[] = "This is my string";
    replace_first(test3, ' ', '_');
    nb_error += TEST_STR(test3, "This_is my string");
    return nb_error;
}

int test_split_on_delimiter()
{
    INIT_TEST_FUNCTION();
    int nb_error = 0;

    char test4[] = "key:value";
    char *key;
    char *value;
    split_on_delimiter(test4, ":", &key, &value);
    nb_error += TEST_STR(key, "key");
    nb_error += TEST_STR(value, "value");

    char test5[] = "key: value";
    split_on_delimiter(test5, ":", &key, &value);
    nb_error += TEST_STR(key, "key");
    nb_error += TEST_STR(value, " value");

    char test6[] = "key:value";
    replace_first(test6, ':', ' ');
    split_on_delimiter(test6, " ", &key, &value);
    nb_error += TEST_STR(key, "key");
    nb_error += TEST_STR(value, "value");

    char test7[] = "";
    split_on_delimiter(test7, ":", &key, &value);
    nb_error += TEST_STR(key, NULL);
    nb_error += TEST_STR(value, NULL);

    char test9[] = "key:value:extra";
    split_on_delimiter(test9, ":", &key, &value);
    nb_error += TEST_STR(key, "key");
    nb_error += TEST_STR(value, "value:extra");

    char test10[] = "key: value :extra";
    split_on_delimiter(test10, ":", &key, &value);
    nb_error += TEST_STR(key, "key");
    nb_error += TEST_STR(value, " value :extra");

    return nb_error;
}

int test_start_with()
{
    INIT_TEST_FUNCTION();
    int nb_error = 0;

    char *prefix = NULL;
    char *string = NULL;
    bool result = false;
    bool _true = true;
    bool _false = false;

    prefix = "Hello";
    string = "Hello World";
    result = start_with(prefix, string);
    nb_error += TEST_BOOLEAN(&result, &_true);

    prefix = "Goodbye";
    string = "Hello World";
    result = start_with(prefix, string);
    nb_error += TEST_BOOLEAN(&result, &_false);

    prefix = "Hello World";
    string = "Hello";
    result = start_with(prefix, string);
    nb_error += TEST_BOOLEAN(&result, &_false);

    prefix = "Hello";
    string = "Hello";
    result = start_with(prefix, string);
    nb_error += TEST_BOOLEAN(&result, &_true);

    prefix = NULL;
    string = "Hello World";
    result = start_with(prefix, string);
    nb_error += TEST_BOOLEAN(&result, &_false);

    return nb_error;
}

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


int test_match()
{
    INIT_TEST_FUNCTION();
    int nb_error = 0;
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
    nb_error += TEST_BOOLEAN(&result, &_true);
    nb_error += TEST_PTR(found_key_finder, &keys[0]);
    nb_error += TEST_STR(raw_value, "value");

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
    nb_error += TEST_BOOLEAN(&result, &_false);
    nb_error += TEST_PTR(found_key_finder, NULL);
    nb_error += TEST_STR(raw_value, NULL);

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
    nb_error += TEST_BOOLEAN(&result, &_false);
    nb_error += TEST_PTR(found_key_finder, NULL);
    nb_error += TEST_STR(raw_value, NULL);

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
    nb_error += TEST_BOOLEAN(&result, &_true);
    nb_error += TEST_PTR(found_key_finder, &keys[1]);
    nb_error += TEST_STR(raw_value, "value");

    // Test 5:
    // -- Setup
    DUMB_KEYFINDER(keys[0], "key", ": ");
    DUMB_PARSER(parser, keys, 1);
    strcpy(line, "");
    found_key_finder = NULL;
    raw_value = NULL;
    // -- Run
    result = match(&parser, line, &found_key_finder, &raw_value);
    nb_error += TEST_BOOLEAN(&result, &_false);
    nb_error += TEST_PTR(found_key_finder, NULL);
    nb_error += TEST_STR(raw_value, NULL);

    return nb_error;
}

int test_info_reader()
{
    INIT_TEST_FILE();
    int nb_error = 0;

    nb_error += test_replace_first();
    nb_error += test_split_on_delimiter();
    nb_error += test_start_with();
    nb_error += test_match();

    return nb_error;
}

#ifdef __TESTING_INFO_READER__
int main()
{
    test_info_reader();
    return 0;
}
#endif

