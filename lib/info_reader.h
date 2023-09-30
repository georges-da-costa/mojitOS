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

#ifndef _INFO_READER_H
#define _INFO_READER_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @struct Parser
 * @brief The parser struct
 * The struct containing all the necessary informations and functions to parse a
 * file
 *
 * @var storage : GenericPointer : pointer to the storage where the parsed data
 * will be stored
 * @var nb_stored : unsigned int : the number of struct stored
 * @var capacity : unsigned int : the maximum number of struct that can be
 * stored
 * @var storage_struct_size : size_t : the size of the struct stored in the
 * storage
 * @var keys : KeyFinder* : pointer to an array of KeyFinder containing the
 * possible keys
 * @var nb_keys : unsigned int : number of key finders
 * @var file : FILE* : pointer to the file that will be parsed
 */
typedef struct Parser Parser;

/**
 * @struct KeyFinder
 * @brief The key finder struct
 * The struct containing all the necessary informations and functions to find a
 * key in a line of text
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
void split_on_delimiter(char *string, const char *delimiter, char **key,
                        char **value);

/**
 * @brief Replace the first occurrence of a character in a string with another
 * character.
 *
 * The function takes a string and two characters as input, and replaces the
 * first occurrence of the first character in the string with the second
 * character.
 *
 * @param[in,out] string The input string where the replacement should take
 * place.
 * @param[in] from The character to be replaced.
 * @param[in] to The character to replace with.
 * @return None.
 */
void replace_first(char *string, char from, char to);

/**
 * @brief Check if a string starts with a prefix.
 *
 * @param[in] prefix The prefix to check.
 * @param[in] string The string to check.
 * @return true The string starts with the prefix.
 * @return false The string does not start with the prefix or one of the input
 * pointers is NULL.
 */
bool start_with(const char *prefix, const char *string);

/**
 * @brief Matches a line of text to a key in the parser's list of keys.
 *
 * @param[in] parser Pointer to the Parser struct.
 * @param[in] line Line of text to match.
 * @param[out] key_finder Pointer to a KeyFinder pointer where the matched key
 * will be stored.
 * @param[out] raw_value Pointer to a char pointer where the value associated
 * with the matched key will be stored.
 *
 * @return Returns 1 if a key is matched, 0 otherwise.
 */
unsigned int match(Parser *parser, char *line, KeyFinder **key_finder,
                   char **raw_value);

/**
* @brief Reads a line of text from a file stream and stores it in a static
  buffer with a maximum size of PAGE_SIZE.

* This function reads a line of text from the input stream pointed to by
* 'stream'. The line of text is stored in a static buffer with a maximum size of
* PAGE_SIZE. The function updates the pointer pointed to by 'lineptr' to point
to
* the buffer containing the line of text. If the line of text is longer than the
* buffer, the function returns -1. If an error occurs,

* @param lineptr A pointer to a pointer to the buffer where the line of text
  will be stored.
* @param stream A pointer to the input stream to read from.
* @return The number of characters read, or -1 if an error occurred the
  function returns -1.
*/
ssize_t buffer_getline(char **lineptr, FILE *stream);

/**
 * @brief Parse with the configured parser.
 *
 * @param parser the parser.
 */
unsigned int parse(Parser *parser);

typedef size_t GenericPointer;
typedef GenericPointer(CopyAllocator)(char *string);
typedef void(Setter)(GenericPointer storage, GenericPointer value);

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

#endif
