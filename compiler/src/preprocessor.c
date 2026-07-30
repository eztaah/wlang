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

    while ((pos = strchr(pos, '\'')) != NULL) {
        char* end_char = strchr(pos + 1, '\'');
        if (!end_char) {
            USER_PANIC(current_filename, current_line_number, "unmatched single quote in character literal");
        }

        int ascii_value = -1;
        char character = *(pos + 1);
        if (character == '\\') {
            // handle escape sequences
            char escape_seq[3] = {pos[1], pos[2], '\0'};
            ascii_value = escape_sequence_to_ascii(escape_seq);
            if (ascii_value == -1) {
                USER_PANIC(current_filename, current_line_number, "invalid escape sequence");
            }
        }
        else {
            // regular character
            ascii_value = (int)character;
        }

        char ascii_str[12]; // enough to hold any ASCII value as string
        sprintf(ascii_str, "%d", ascii_value);

        // replace the character or escape sequence with its ASCII value
        str_remove_range(source, pos - src, end_char + 1 - src);
        str_insert(source, pos - src, ascii_str);

        // update the src pointer because we've modified the source
        src = str_to_char(source);
    }
}

static void convert_string_literals_to_ascii_array(Str* source)
{
    print(VERBOSE, 2, "converting string literals to ASCII arrays\n");
    char* src = str_to_char(source);
    char* pos = src;

    while ((pos = strchr(pos, '"')) != NULL) {
        char* end_string = strchr(pos + 1, '"');
        if (!end_string) {
            USER_PANIC(current_filename, current_line_number, "unmatched double quote in string literal");
        }

        Str* ascii_array = str_new("[");
        for (char* p = pos + 1; p < end_string; p++) {
            int ascii_value = -1;
            if (*p == '\\') {
                // handle escape sequences
                char escape_seq[3] = {p[0], p[1], '\0'};
                ascii_value = escape_sequence_to_ascii(escape_seq);
                if (ascii_value == -1) {
                    USER_PANIC(current_filename, current_line_number, "invalid escape sequence");
                }
                p++; // skip the next character as it's part of the escape sequence
            }
            else {
                ascii_value = (int)*p;
            }

            char ascii_str[12]; // enough to hold any ASCII value as string
            sprintf(ascii_str, "%d", ascii_value);
            str_cat(ascii_array, ascii_str);
            if (p < end_string - 1) {
                str_cat(ascii_array, ", ");
            }
        }
        str_cat(ascii_array, "]"); // close the array

        // replace the string literal with the ASCII array
        str_remove_range(source, pos - src, end_string + 1 - src);
        str_insert(source, pos - src, str_to_char(ascii_array));

        // update the src pointer because we modified the source
        src = str_to_char(source);
        str_free(ascii_array);
    }
}

static void collect_macros(Str* source, Dict* macro_dict)
{
    print(VERBOSE, 2, "collecting macros\n");
    char* src = str_to_char(source);
    char* pos = src;

    while ((pos = strstr(pos, "#def")) != NULL) {
        char* end_line = strchr(pos, '\n');
        if (!end_line) {
            break;
        }

        char* macro_start = pos + 5; // start after "#def "
        char* macro_end = strchr(macro_start, ' ');

        // handle macros with or without values
        char* macro_value = NULL;
        if (macro_end && macro_end < end_line) {
            char* value_start = macro_end + 1;
            macro_value = strndup(value_start, end_line - value_start);
        }
        else {
            // handle case where macro doesn't have value
            macro_end = end_line;
            macro_value = strdup("1"); // use 1 as the default value
        }

        char* macro_name = strndup(macro_start, macro_end - macro_start);

        dict_put(macro_dict, macro_name, str_to_char(str_new(macro_value)));
        print(VERBOSE, 3, "collect macro: %s = %s\n", macro_name, macro_value);

        free(macro_name);
        free(macro_value);

        // remove the #def line from the source
        str_remove_range(source, pos - src, end_line + 1 - src);

        // update the src pointer since we have modified the source
        src = str_to_char(source);
        advance_to_next_line(&pos);
    }
}

static void replace_macros(Str* source, Dict* macro_dict)
{
    print(VERBOSE, 2, "replacing macros\n");
    for (int i = 0; i < macro_dict->size; ++i) {
        DictEntry* entry = macro_dict->entries[i];
        str_replace(source, entry->key, entry->value);
        print(VERBOSE, 3, "replace macro: %s with %s\n", entry->key, entry->value);
    }
}

static void process_conditionals(Str* source, Dict* macro_dict)
{
    print(VERBOSE, 2, "processing conditionals\n");
    char* src = str_to_char(source);
    char* pos = src;

    while ((pos = strstr(pos, "#ifdef")) != NULL) {
        char* end_line = strchr(pos, '\n');
        if (!end_line) {
            USER_PANIC(current_filename, current_line_number, "unexpected end of file after #ifdef");
        }

        // extract the macro name
        char* macro_start = pos + 7; // start after "#ifdef "
        char* macro_end = end_line;
        char* macro_name = strndup(macro_start, macro_end - macro_start);
        Bool macro_defined = dict_get(macro_dict, macro_name) != NULL;

        print(VERBOSE, 3, "processing #ifdef for macro: %s\n", macro_name);

        // find positions of #else and #endif
        char* else_pos = strstr(end_line, "#else");
        char* endif_pos = strstr(end_line, "#endif");

        if (!endif_pos) {
            USER_PANIC(current_filename, current_line_number, "missing #endif for #ifdef");
        }

        if (macro_defined) {
            print(VERBOSE, 3, "macro %s is defined\n", macro_name);
            // if macro is defined, remove the #else block if it exists
            if (else_pos && else_pos < endif_pos) {
                str_remove_range(source, else_pos - src, endif_pos + 6 - src); // remove from #else to #endif
            }
            str_remove_range(source, pos - src, end_line + 1 - src); // remove the #ifdef line
        }
        else {
            print(VERBOSE, 3, "macro %s is not defined\n", macro_name);
            // if macro is not defined, remove the #ifdef block and keep #else or remove all if no #else
            if (else_pos && else_pos < endif_pos) {
                str_remove_range(source, pos - src, else_pos + 6 - src); // remove from #ifdef to #else
            }
            else {
                str_remove_range(source, pos - src, endif_pos + 6 - src); // remove from #ifdef to #endif
            }
        }

        // remove the #endif line
        endif_pos = strstr(str_to_char(source), "#endif");
        if (endif_pos) {
            char* end_of_endif_line = strchr(endif_pos, '\n');
            if (end_of_endif_line) {
                str_remove_range(source, endif_pos - str_to_char(source), end_of_endif_line + 1 - str_to_char(source));
            }
            else {
                str_remove_range(source, endif_pos - str_to_char(source), strlen(str_to_char(source)));
            }
        }

        // update the `src` pointer since we've modified the source
        src = str_to_char(source);
        advance_to_next_line(&pos);


        free(macro_name);
    }
}

static void process_includes(Str* source)
{
    print(VERBOSE, 2, "processing includes\n");
    char* src = str_to_char(source);
    char* pos = src;

    while ((pos = strstr(pos, "#incl")) != NULL) {
        char* end_line = strchr(pos, '\n');
        if (!end_line) {
            break;
        }

        // extract file name
        char* file_start = pos + 6; // Start after "#incl "
        char* file_end = end_line;
        char* file_name = strndup(file_start, file_end - file_start);

        print(VERBOSE, 3, "including file: %s\n", file_name);

        // read the content of the included file
        char* included_content = read_file(file_name);
        if (!included_content) {
            USER_PANIC(current_filename, current_line_number, "failed to include file: %s", file_name);
        }

        // replace the #incl line with the content of the file
        Str* included_str = str_new(included_content);
        str_remove_range(source, pos - src, end_line + 1 - src);
        str_insert(source, pos - src, included_content);

        free(file_name);
        free(included_content);
        str_free(included_str);

        advance_to_next_line(&pos);
    }
}

static void remove_comments(Str* source)
{
    print(VERBOSE, 2, "removing comments\n");
    char* src = str_to_char(source);
    char* pos = src;
    Bool in_string = FALSE;
    Bool in_char = FALSE;

    while ((pos = strchr(pos, ':')) != NULL) {
        // check if ':' is inside a string or a character
        char* scan_pos = src;
        while (scan_pos < pos) {
            if (*scan_pos == '"' && (scan_pos == src || *(scan_pos - 1) != '\\')) {
                in_string = !in_string;
            }
            else if (*scan_pos == '\'' && (scan_pos == src || *(scan_pos - 1) != '\\')) {
                in_char = !in_char; 
            }
            scan_pos++;
        }

        // if ':' is inside a string or character, skip it
        if (in_string || in_char) {
            pos++;
            continue;
        }

        // ':' is not inside a string or character, so it is a comment
        char* end_line = strchr(pos, '\n');
        if (!end_line) {
            str_remove_range(source, pos - src, source->length); // remove from ':' to end of string
            break;
        }
        else {
            str_remove_range(source, pos - src, end_line - src); // remove the comment
        }
    }
}

static void remove_annotations(Str* source)
{
    print(VERBOSE, 2, "removing annotations\n");
    char* src = str_to_char(source);
    char* pos = src;
    Bool in_string = FALSE;
    Bool in_char = FALSE;

    while ((pos = strchr(pos, '!')) != NULL) {
        // check if '!' is inside a string, character, or is part of '!='
        char* scan_pos = src;
        while (scan_pos < pos) {
            if (*scan_pos == '"' && (scan_pos == src || *(scan_pos - 1) != '\\')) {
                in_string = !in_string; 
            }
            else if (*scan_pos == '\'' && (scan_pos == src || *(scan_pos - 1) != '\\')) {
                in_char = !in_char; 
            }
            scan_pos++;
        }

        // if '!' is inside a string, character, or followed by '=', skip it
        if (in_string || in_char || (*(pos + 1) == '=')) {
            pos++; 
            continue;
        }

        // process the annotation
        if (pos[1] == ' ' || pos[1] == '\t') { // skip the entire line if ! is followed by a space
            char* end_line = strchr(pos, '\n');
            if (!end_line) {
                str_remove_range(source, pos - src, source->length);
                break;
            }
            else {
                str_remove_range(source, pos - src, end_line + 1 - src);
            }
        } 
        else { // Skip the word until the next space or tab
            char* space = pos + 1;
            while (*space != ' ' && *space != '\t' && *space != '\n' && *space != '\0') {
                space++;
            }

            str_remove_range(source, pos - src, space - src);
        }
    }
}

Str* preprocess_file(const char* filename, Dict* macro_dict)
{
    char* raw_content = read_file(filename);

    Str* source = str_new(raw_content);
    free(raw_content);

    remove_comments(source);
    remove_annotations(source);

    process_includes(source);
    
    collect_macros(source, macro_dict);
    process_conditionals(source, macro_dict);
    replace_macros(source, macro_dict); 

    convert_chars_to_ascii(source);
    convert_string_literals_to_ascii_array(source);

    remove_comments(source); // remove comments and annotations again because the included file can contain comments and annotations
    remove_annotations(source);

    str_cat(source, "\n");  // prevent incorrect lexing 

    return source;
}

