#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @struct Parser
 * @brief The parser struct
 * The struct containing all the necessary informations and functions to parse a file
 *
 * @var storage : GenericPointer : pointer to the storage where the parsed data will be stored
 * @var nb_stored : unsigned int : the number of struct stored
 * @var capacity : unsigned int : the maximum number of struct that can be stored
 * @var storage_struct_size : size_t : the size of the struct stored in the storage
 * @var keys : KeyFinder* : pointer to an array of KeyFinder containing the possible keys
 * @var nb_keys : unsigned int : number of key finders
 * @var file : FILE* : pointer to the file that will be parsed
*/
typedef struct Parser Parser;

/**
 * @struct KeyFinder
 * @brief The key finder struct
 * The struct containing all the necessary informations and functions to find a key in a line of text
 *
 * @var key : char* : the key to be found
 * @var delimiter : char* : the delimiter between the key and the value
 * @var copy : CopyAllocator*: the function to use to make a copy of the value
 * @var set : Setter*: the function to use to store the value in the storage
*/
typedef struct KeyFinder KeyFinder;

/**
 * @brief Split a string into a key and value based on a specified delimiter.
 *
 * The function takes a string and splits it into two parts: a key and a value.
 * The parts are determined by a specified delimiter.
 * The key and value are returned through output parameters.
 * If the delimiter is not found in the input string, the key and value pointers
 * will be set to NULL.
 *
 * @param[in] string The input string to be split, the string is modified.
 * @param[in] delimiter The delimiter string.
 * @param[out] key A pointer to a char pointer where the key will be stored.
 * @param[out] value A pointer to a char pointer where the value will be stored.
 * @return None.
 */
static void split_on_delimiter(char *string, const char *delimiter, char **key, char **value);

/**
 * @brief Replace the first occurrence of a character in a string with another character.
 *
 * The function takes a string and two characters as input, and replaces the first
 * occurrence of the first character in the string with the second character.
 *
 * @param[in,out] string The input string where the replacement should take place.
 * @param[in] from The character to be replaced.
 * @param[in] to The character to replace with.
 * @return None.
 */
static void replace_first(char *string, char from, char to);

/**
 * @brief Check if a string starts with a prefix.
 *
 * @param[in] prefix The prefix to check.
 * @param[in] string The string to check.
 * @return true The string starts with the prefix.
 * @return false The string does not start with the prefix or one of the input pointers is NULL.
 */
static bool start_with(const char *prefix, const char *string);

/**
 * @brief Matches a line of text to a key in the parser's list of keys.
 *
 * @param parser Pointer to the Parser struct.
 * @param line Line of text to match.
 * @param key_finder Pointer to a KeyFinder pointer where the matched key will be stored.
 * @param raw_value Pointer to a char pointer where the value associated with the matched key will be stored.
 *
 * @return Returns 1 if a key is matched, 0 otherwise.
 */
static unsigned int match(Parser *parser, char *line, KeyFinder **key_finder, char **raw_value);

typedef size_t GenericPointer;
typedef GenericPointer (CopyAllocator) (char *string);
typedef void (Setter) (GenericPointer storage, GenericPointer value);

struct KeyFinder {
    char *key;
    char *delimiter;

    CopyAllocator *copy;
    Setter *set;
};

struct Parser {
    GenericPointer storage;
    unsigned int nb_stored;
    unsigned int capacity;
    size_t storage_struct_size;

    KeyFinder *keys;
    unsigned int nb_keys;

    FILE *file;
};

static void set_value(Parser *parser, KeyFinder *key_finder, char *raw_value)
{
    GenericPointer address = parser->storage + (parser->storage_struct_size * parser->nb_stored);
    GenericPointer value = key_finder->copy(raw_value);
    key_finder->set(address, value);
}

// static void storage_zero(Parser *parser) {
// 	static const int zero = 0;
// 	GenericPointer storage = parser->storage;
// 	size_t capacity = parser->capacity;
// 	size_t struct_size = parser->storage_struct_size;
// 	memset(storage, zero, struct_size * capacity);
// }

static unsigned int match(Parser *parser, char *line, KeyFinder **key_finder, char **raw_value)
{
    for (unsigned int i = 0; i < parser->nb_keys; i++) {
        KeyFinder *finder = &parser->keys[i];

        if (start_with(finder->key, line)) {
            char *value = NULL;
            char *key = NULL;

            split_on_delimiter(line, finder->delimiter, &key, &value);
            if ( key == NULL || value == NULL) {
                return 0;
            }
            *key_finder = finder;
            *raw_value = value;
            return 1;
        }
    }
    return 0;
}

static unsigned int move_to_next(Parser *parser)
{
    parser->nb_stored += 1;
    if (parser->nb_stored >= parser->capacity) {
        return 0;
    }
    return 1;
}

static unsigned int parse(Parser *parser)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    unsigned int key_assigned = 0;

    while ((read = getline(&line, &len, parser->file)) != -1) {
        if (key_assigned == parser->nb_keys && read > 1) {
            continue;
        } else if (read == 1) {
            if (!move_to_next(parser)) {
                return 0;
            }
            key_assigned = 0;
        } else {
            KeyFinder *key_finder = NULL;
            char *raw_value = NULL;
            replace_first(line, '\n', '\0');
            if (match(parser, line, &key_finder, &raw_value)) {
                set_value(parser, key_finder, raw_value);
                ++key_assigned;
            }
        }
    }
    if (key_assigned > 0) {
        parser->nb_stored++;
    }
    free(line);
    return 1;
}



static void replace_first(char *string, char from, char to)
{
    for (int i = 0; string[i] != '\0'; i++) {
        if (string[i] == from) {
            string[i] = to;
            break;
        }
    }
}

static void split_on_delimiter(char *string, const char *delimiter, char **key, char **value)
{
    *key = NULL;
    *value = NULL;
    size_t delimiter_len = strlen(delimiter);
    char *start_delimiter = strstr(string, delimiter);
    if (start_delimiter != NULL) {
        *start_delimiter = '\0';
        *key = string;
        *value = start_delimiter + delimiter_len;
    }
}

static bool start_with(const char *prefix, const char *string)
{
    if (prefix == NULL || string == NULL) {
        return false;
    }

    size_t prefix_len = strlen(prefix);
    size_t string_len = strlen(string);

    if (string_len < prefix_len) {
        return false;
    } else {
        return  memcmp(prefix, string, prefix_len) == 0;
    }
}

