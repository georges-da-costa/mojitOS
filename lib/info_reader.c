#include <info_reader.h>

 void set_value(Parser *parser, KeyFinder *key_finder, char *raw_value)
{
    GenericPointer address = parser->storage + (parser->storage_struct_size * parser->nb_stored);
    GenericPointer value = key_finder->copy(raw_value);
    key_finder->set(address, value);
}

 unsigned int match(Parser *parser, char *line, KeyFinder **key_finder, char **raw_value)
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

 unsigned int move_to_next(Parser *parser)
{
    parser->nb_stored += 1;
    if (parser->nb_stored >= parser->capacity) {
        return 0;
    }
    return 1;
}


#define MAX_PAGE_SIZE 4096
ssize_t buffer_getline(char **lineptr, FILE *stream) {
    ssize_t num_chars_read = 0;
    static char buffer[MAX_PAGE_SIZE] = {0};

    if (!lineptr || !stream) {
        return -1;
    }

    while (1) {
        int ch = fgetc(stream);
        if (ch == EOF) {
            if (num_chars_read == 0) {
                return -1;
            } else {
                break;
            }
        }

        if (num_chars_read == MAX_PAGE_SIZE - 1) {
            return -1;
        }

        buffer[num_chars_read++] = ch;
        if (ch == '\n') {
            break;
        }
    }

    buffer[num_chars_read] = '\0';
    *lineptr = buffer;

    return num_chars_read;
}


 unsigned int parse(Parser *parser)
{
    char *line = NULL;
    ssize_t read;
    unsigned int key_assigned = 0;

    while ((read = buffer_getline(&line, parser->file)) != -1) {
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
    return 1;
}



 void replace_first(char *string, char from, char to)
{
    for (int i = 0; string[i] != '\0'; i++) {
        if (string[i] == from) {
            string[i] = to;
            break;
        }
    }
}

 void split_on_delimiter(char *string, const char *delimiter, char **key, char **value)
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

bool start_with(const char *prefix, const char *string)
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
