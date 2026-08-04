#include "lib.h"
#include <stdio.h>  // sprintf()
#include <stdlib.h> // malloc(), free()
#include <string.h> // strstr(), strchr(), strlen(), strcpy(), strcat()

#include "compiler.h"

static int current_line_number = 1;

static int escape_sequence_to_ascii(char* seq)
{
    if (strcmp(seq, "\\n") == 0) {
        return 10;
    }
    if (strcmp(seq, "\\0") == 0) {
        return 0;
    }
    if (strcmp(seq, "\\t") == 0) {
        return 9;
    }
    if (strcmp(seq, "\\r") == 0) {
        return 13;
    }
    if (strcmp(seq, "\\\\") == 0) {
        return 92;
    }
    if (strcmp(seq, "\\\'") == 0) {
        return 39;
    }
    if (strcmp(seq, "\\\"") == 0) {
        return 34;
    }
    return -1;
}

static void advance_to_next_line(char** pos)
{
    while (**pos != '\n' && **pos != '\0') {
        (*pos)++;
    }
    if (**pos == '\n') {
        current_line_number++;
        (*pos)++;
    }
}

static void convert_chars_to_ascii(Str* source)
{
    print(VERBOSE, 2, "converting character literals to ASCII\n");
    char* src = str_to_char(source);
    char* pos = src;

    while ((pos = strchr(pos, '\'')) != NULL) { // find the next single quote
        char* end_char = strchr(pos + 1, '\'');
        if (!end_char) {
            USER_PANIC(current_filename, current_line_number, "unmatched single quote in character literal");
        }

        int ascii_value = -1;
        char character = *(pos + 1);
        if (character == '\\') {
            char escape_seq[3] = {pos[1], pos[2], '\0'};
            ascii_value = escape_sequence_to_ascii(escape_seq);
            if (ascii_value == -1) {
                USER_PANIC(current_filename, current_line_number, "invalid escape sequence");
            }
        }
        else {
            ascii_value = (int)character;
        }

        char ascii_str[12];
        sprintf(ascii_str, "%d", ascii_value);

        str_remove_range(source, pos - src, end_char + 1 - src); // remove the character literal
        str_insert(source, pos - src, ascii_str);                // insert the ASCII value

        src = str_to_char(source); // update src in case the string was reallocated
    }
}

static void convert_string_literals_to_ascii_array(Str* source)
{
    print(VERBOSE, 2, "converting string literals to ASCII arrays\n");
    char* src = str_to_char(source);
    char* pos = src;

    while ((pos = strchr(pos, '"')) != NULL) { // find the next double quote
        char* end_string = strchr(pos + 1, '"');
        if (!end_string) {
            USER_PANIC(current_filename, current_line_number, "unmatched double quote in string literal");
        }

        Str* ascii_array = str_new("[");
        for (char* p = pos + 1; p < end_string; p++) {
            int ascii_value = -1;
            if (*p == '\\') {
                char escape_seq[3] = {p[0], p[1], '\0'};
                ascii_value = escape_sequence_to_ascii(escape_seq);
                if (ascii_value == -1) {
                    USER_PANIC(current_filename, current_line_number, "invalid escape sequence");
                }
                p++; // skip the next character because it's part of the escape sequence
            }
            else {
                ascii_value = (int)*p;
            }

            char ascii_str[12];
            sprintf(ascii_str, "%d", ascii_value);
            str_cat(ascii_array, ascii_str); // add the ASCII value to the array
            if (p < end_string - 1) {
                str_cat(ascii_array, ", "); // add a comma separator
            }
        }

        str_cat(ascii_array, "]"); // close the array
        int start_pos = pos - src;
        int end_pos = end_string + 1 - src;

        str_remove_range(source, start_pos, end_pos);            // remove the string literal
        str_insert(source, start_pos, str_to_char(ascii_array)); // insert the ASCII array

        src = str_to_char(source); // update src in case the string was reallocated
        pos = src + start_pos + ascii_array->length;
        str_free(ascii_array);
    }
}

// collects and stores macros defined in the source code
// stores them in the provided dictionary
static void collect_macros(Str* source, Dict* macro_dict)
{
    print(VERBOSE, 2, "collecting macros\n");
    char* src = str_to_char(source);
    char* pos = src;

    while ((pos = strstr(pos, "#def")) != NULL) { // find the next macro definition
        char* end_line = strchr(pos, '\n');
        if (!end_line) {
            break;
        }

        // trim leading spaces and tabs to find the start of the macro name
        char* macro_start = pos + 4;
        while (*macro_start == ' ' || *macro_start == '\t') {
            macro_start++;
        }

        // find the end of the macro name
        char* macro_end = macro_start;
        while (*macro_end != ' ' && *macro_end != '\t' && *macro_end != '\n') {
            macro_end++;
        }

        // find the start of the macro value
        char* value_start = macro_end;
        while (*value_start == ' ' || *value_start == '\t') {
            value_start++;
        }

        // extract the macro name and value
        char* macro_name = strndup(macro_start, macro_end - macro_start);
        char* macro_value = strndup(value_start, end_line - value_start);

        dict_put(macro_dict, macro_name, macro_value); // store the macro in the dictionary
        print(VERBOSE, 3, "collect macro: %s = %s\n", macro_name, macro_value);

        free(macro_name);
        free(macro_value);

        str_remove_range(source, pos - src, end_line - src); // remove the macro definition from the source code
        src = str_to_char(source);                           // update src in case the string was reallocated
    }
}

// replaces all macros in the source code with their corresponding values
// uses the dictionary of macros collected earlier
static void replace_macros(Str* source, Dict* macro_dict)
{
    print(VERBOSE, 2, "replacing macros\n");
    for (int i = 0; i < macro_dict->size; ++i) {
        DictEntry* entry = macro_dict->entries[i];
        str_replace(source, entry->key, entry->value); // replace the macro with its value
        print(VERBOSE, 3, "replace macro: %s with %s\n", entry->key, entry->value);
    }
}

// processes #if, #ifdef, and #ifndef conditionals in the source code
static void process_conditionals(Str* source, Dict* macro_dict)
{
    print(VERBOSE, 2, "processing conditionals\n");
    char* src = str_to_char(source);
    char* pos = src;

    while ((pos = strstr(pos, "#if")) != NULL) {
        Bool is_ifndef = (strncmp(pos, "#ifndef", 7) == 0); // check if it's an #ifndef directive
        char* end_line = strchr(pos, '\n');
        if (!end_line) {
            USER_PANIC(current_filename, current_line_number, "unexpected end of file after #if directive");
        }

        // extract the macro name
        char* macro_start = pos + (is_ifndef ? 8 : 7); // 7 for #ifdef, 8 for #ifndef
        char* macro_end = end_line;

        // trim leading and trailing spaces
        while (*macro_start == ' ' || *macro_start == '\t') {
            macro_start++;
        }
        while (*(macro_end - 1) == ' ' || *(macro_end - 1) == '\t') {
            macro_end--;
        }

        // check if the macro is defined
        char* macro_name = strndup(macro_start, macro_end - macro_start);
        Bool macro_defined = dict_get(macro_dict, macro_name) != NULL;

        // for #ifndef, invert the macro_defined check
        if (is_ifndef) {
            macro_defined = !macro_defined;
        }

        print(VERBOSE, 3, "processing %s for macro: %s\n", is_ifndef ? "#ifndef" : "#ifdef", macro_name);

        // find positions of #else and #endif
        char* else_pos = strstr(end_line, "#else");
        char* endif_pos = strstr(end_line, "#endif");

        if (!endif_pos) {
            USER_PANIC(current_filename, current_line_number, "missing #endif for #if directive");
        }

        int newline_count = 0;

        if (macro_defined) {
            print(VERBOSE, 3, "macro %s condition is true\n", macro_name);

            // if macro condition is true, remove the #else block if it exists
            if (else_pos && else_pos < endif_pos) {
                // count newlines in the #else block
                char* tmp_pos = else_pos;
                while (tmp_pos < endif_pos) {
                    if (*tmp_pos == '\n') {
                        newline_count++;
                    }
                    tmp_pos++;
                }

                // remove from #else to #endif and insert the newlines
                str_remove_range(source, else_pos - src, endif_pos + 6 - src);
                for (int i = 0; i < newline_count; i++) {
                    str_insert(source, else_pos - src, "\n");
                }
            }
            else {
                // just remove the #endif line
                endif_pos = strstr(str_to_char(source), "#endif");
                if (endif_pos) {
                    char* end_of_endif_line = strchr(endif_pos, '\n');
                    if (end_of_endif_line) {
                        str_remove_range(source, endif_pos - str_to_char(source), end_of_endif_line - str_to_char(source));
                    }
                    else {
                        str_remove_range(source, endif_pos - str_to_char(source), strlen(str_to_char(source)));
                    }
                }
            }

            // remove the #if line
            str_remove_range(source, pos - src, end_line + 1 - src);
            // insert a newline after removing #if line
            str_insert(source, pos - src, "\n");
        }
        else {
            print(VERBOSE, 3, "macro %s condition is false\n", macro_name);

            // count newlines in the #if block
            char* tmp_pos = end_line;
            while (tmp_pos < (else_pos ? else_pos : endif_pos)) {
                if (*tmp_pos == '\n') {
                    newline_count++;
                }
                tmp_pos++;
            }

            // if macro condition is false, remove the #if block and keep #else or remove all if no #else
            if (else_pos && else_pos < endif_pos) {
                str_remove_range(source, pos - src, else_pos + 6 - src);
            }
            else {
                str_remove_range(source, pos - src, endif_pos + 6 - src);
            }

            // insert the newlines after removal
            for (int i = 0; i < newline_count + 1; i++) {
                str_insert(source, pos - src, "\n");
            }

            // remove the #endif line
            endif_pos = strstr(str_to_char(source), "#endif");
            if (endif_pos) {
                char* end_of_endif_line = strchr(endif_pos, '\n');
                if (end_of_endif_line) {
                    str_remove_range(source, endif_pos - str_to_char(source), end_of_endif_line - str_to_char(source));
                }
                else {
                    str_remove_range(source, endif_pos - str_to_char(source), strlen(str_to_char(source)));
                }
            }
        }

        // update the `src` pointer since we've modified the source
        src = str_to_char(source);
        advance_to_next_line(&pos);

        free(macro_name);
    }
}

static void remove_comments(Str* source)
{
    print(VERBOSE, 2, "removing comments\n");
    char* src = str_to_char(source);
    char* pos = src;
    Bool in_string = FALSE;
    Bool in_char = FALSE;

    while (*pos != '\0') {
        // track whether we're inside a string or character literal
        if (*pos == '"' && (pos == src || *(pos - 1) != '\\')) {
            in_string = !in_string;
        }
        else if (*pos == '\'' && (pos == src || *(pos - 1) != '\\')) {
            in_char = !in_char;
        }

        // if we find a ':' that is not inside a string or char literal, treat it as a comment
        if (*pos == ':' && !in_string && !in_char) {
            char* end_line = strchr(pos, '\n');
            if (end_line) {
                // preserve the newline, remove everything up to it
                str_remove_range(source, pos - src, end_line - src);
            }
            else {
                // no newline, remove until the end of the source
                str_remove_range(source, pos - src, source->length);
                break;
            }
        }

        pos++;
    }
}

static void remove_annotations(Str* source)
{
    print(VERBOSE, 2, "removing annotations\n");
    char* src = str_to_char(source);
    char* pos = src;
    Bool in_string = FALSE;
    Bool in_char = FALSE;

    while (*pos != '\0') {
        // track whether we're inside a string or character literal
        if (*pos == '"' && (pos == src || *(pos - 1) != '\\')) {
            in_string = !in_string;
        }
        else if (*pos == '\'' && (pos == src || *(pos - 1) != '\\')) {
            in_char = !in_char;
        }

        // handle '!' that should not be removed as an annotation
        if (*pos == '!' && !in_string && !in_char) {
            // check if '!' is followed by '='
            if (*(pos + 1) == '=') {
                pos += 2; // skip the '!=' operator
                continue;
            }

            // proceed to remove the word following '!', as it's an annotation
            char* word_start = pos + 1;
            while (*word_start == ' ' || *word_start == '\t') {
                word_start++;
            }

            char* word_end = word_start;
            while (*word_end != ' ' && *word_end != '\t' && *word_end != '\n' && *word_end != '\0' && *word_end != '<') {
                word_end++;
            }

            // remove the annotation
            str_remove_range(source, pos - src, word_end - src + 1);

            // update position pointer
            pos = src + (word_end - src) - 1;
            continue;
        }

        pos++;
    }
}

Str* preprocess_file(const char* filename, Dict* macro_dict)
{
    char* raw_content = read_file(filename);

    Str* source = str_new(raw_content);
    free(raw_content);

    remove_comments(source);
    remove_annotations(source);

    collect_macros(source, macro_dict);
    process_conditionals(source, macro_dict);
    replace_macros(source, macro_dict);

    convert_chars_to_ascii(source);
    convert_string_literals_to_ascii_array(source);

    remove_comments(source);
    remove_annotations(source);

    str_cat(source, "\n"); // add a newline at the end to prevent incorrect lexing

    return source;
}
