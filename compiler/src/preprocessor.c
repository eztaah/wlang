#include "lib.h"
#include <stdlib.h> // malloc(), free()
#include <string.h> // strstr(), strchr(), strlen(), strcpy(), strcat()
#include <stdio.h>


static I32 escape_sequence_to_ascii(Char* seq) {
    if (strcmp(seq, "\\n") == 0) return 10;    // Newline
    if (strcmp(seq, "\\0") == 0) return 0;     // Null terminator
    if (strcmp(seq, "\\t") == 0) return 9;     // Tab
    if (strcmp(seq, "\\r") == 0) return 13;    // Carriage return
    if (strcmp(seq, "\\\\") == 0) return 92;   // Backslash
    if (strcmp(seq, "\\\'") == 0) return 39;   // Single quote
    if (strcmp(seq, "\\\"") == 0) return 34;   // Double quote
    return -1; // Invalid escape sequence
}

static Void convert_chars_to_ascii(Str* source) {
    Char* src = str_to_char(source);
    Char* pos = src;

    while ((pos = strchr(pos, '\'')) != NULL) {
        Char* end_char = strchr(pos + 1, '\'');
        if (!end_char) {
            PANIC("Unmatched single quote in character literal");
        }

        I32 ascii_value = -1;
        Char character = *(pos + 1);
        if (character == '\\') {
            // Handle escape sequences
            Char escape_seq[3] = {pos[1], pos[2], '\0'};
            ascii_value = escape_sequence_to_ascii(escape_seq);
            if (ascii_value == -1) {
                PANIC("Invalid escape sequence");
            }
        } else {
            // Regular character
            ascii_value = (I32)character;
        }

        Char ascii_str[12]; // Enough to hold any ASCII value as string
        sprintf(ascii_str, "%d", ascii_value);

        // Replace the character or escape sequence with its ASCII value
        str_remove_range(source, pos - src, end_char + 1 - src);
        str_insert(source, pos - src, ascii_str);

        // Update the src pointer since we've modified the source
        src = str_to_char(source);
    }
}

static Void convert_string_literals_to_ascii_array(Str* source) {
    Char* src = str_to_char(source);
    Char* pos = src;

    while ((pos = strchr(pos, '"')) != NULL) {
        Char* end_string = strchr(pos + 1, '"');
        if (!end_string) {
            PANIC("Unmatched double quote in string literal");
        }

        Str* ascii_array = str_new("[");
        for (Char* p = pos + 1; p < end_string; p++) {
            I32 ascii_value = -1;
            if (*p == '\\') {
                // Handle escape sequences
                Char escape_seq[3] = {p[0], p[1], '\0'};
                ascii_value = escape_sequence_to_ascii(escape_seq);
                if (ascii_value == -1) {
                    PANIC("Invalid escape sequence");
                }
                p++; // Skip the next character as it's part of the escape sequence
            } else {
                ascii_value = (I32)*p;
            }

            Char ascii_str[12]; // Enough to hold any ASCII value as string
            sprintf(ascii_str, "%d", ascii_value);
            str_cat(ascii_array, ascii_str);
            if (p < end_string - 1) {
                str_cat(ascii_array, ", ");
            }
        }
        str_cat(ascii_array, "]"); // close the array

        // Replace the string literal with the ASCII array
        str_remove_range(source, pos - src, end_string + 1 - src);
        str_insert(source, pos - src, str_to_char(ascii_array));

        // Update the src pointer since we've modified the source
        src = str_to_char(source);

        // Free the temporary Str
        str_free(ascii_array);
    }
}

// Collect and store all macros in the dictionary
static Void collect_macros(Str* source, DictStr* macro_dict)
{
    Char* src = str_to_char(source);
    Char* pos = src;

    while ((pos = strstr(pos, "#def")) != NULL) {
        Char* end_line = strchr(pos, '\n');
        if (!end_line)
            break;

        // Extract macro name
        Char* macro_start = pos + 5; // Start after "#def "
        Char* macro_end = strchr(macro_start, ' ');

        // Handle macros with or without values
        Char* macro_value = NULL;
        if (macro_end && macro_end < end_line) {
            // Macro with a value
            Char* value_start = macro_end + 1;
            macro_value = strndup(value_start, end_line - value_start);
        } else {
            // Macro without a value (e.g., #def DEV_MODE)
            macro_end = end_line;
            macro_value = strdup("1");  // Use "1" as the default value
        }

        Char* macro_name = strndup(macro_start, macro_end - macro_start);

        // Store the macro in the dictionary
        dictstr_put(macro_dict, macro_name, str_to_char(str_new(macro_value)));

        free(macro_name);
        free(macro_value);

        // Remove the #def line from the source
        str_remove_range(source, pos - src, end_line + 1 - src);

        // Update the src pointer since we've modified the source
        src = str_to_char(source);
    }
}

// Replace macros in the source string with their values
static Void replace_macros(Str* source, DictStr* macro_dict)
{
    // Replace macros in the source string
    for (I32 i = 0; i < macro_dict->size; ++i) {
        DictStrEntry* entry = macro_dict->entries[i];
        str_replace(source, entry->key, entry->value);
    }
}

static Void process_conditionals(Str* source, DictStr* macro_dict)
{
    Char* src = str_to_char(source);
    Char* pos = src;

    while ((pos = strstr(pos, "#ifdef")) != NULL) {
        Char* end_line = strchr(pos, '\n');
        if (!end_line) {
            PANIC("Unexpected end of file after #ifdef");
        }

        // Extract the macro name
        Char* macro_start = pos + 7; // Start after "#ifdef "
        Char* macro_end = end_line;
        Char* macro_name = strndup(macro_start, macro_end - macro_start);
        Bool macro_defined = dictstr_get(macro_dict, macro_name) != NULL;
        free(macro_name);

        // Find positions of #else and #endif
        Char* else_pos = strstr(end_line, "#else");
        Char* endif_pos = strstr(end_line, "#endif");

        if (!endif_pos) {
            PANIC("Missing #endif for #ifdef");
        }

        if (macro_defined) {
            // If macro is defined, remove the #else block if it exists
            if (else_pos && else_pos < endif_pos) {
                str_remove_range(source, else_pos - src, endif_pos + 6 - src); // Remove from #else to #endif
            }
            str_remove_range(source, pos - src, end_line + 1 - src); // Remove the #ifdef line
        } else {
            // If macro is not defined, remove the #ifdef block and keep #else or remove all if no #else
            if (else_pos && else_pos < endif_pos) {
                str_remove_range(source, pos - src, else_pos + 6 - src); // Remove from #ifdef to #else
            } else {
                str_remove_range(source, pos - src, endif_pos + 6 - src); // Remove from #ifdef to #endif
            }
        }

        // Finally, remove the #endif line
        endif_pos = strstr(str_to_char(source), "#endif");
        if (endif_pos) {
            Char* end_of_endif_line = strchr(endif_pos, '\n');
            if (end_of_endif_line) {
                str_remove_range(source, endif_pos - str_to_char(source), end_of_endif_line + 1 - str_to_char(source));
            } else {
                str_remove_range(source, endif_pos - str_to_char(source), strlen(str_to_char(source)));
            }
        }

        // Update the `src` pointer since we've modified the source
        src = str_to_char(source);
        pos = src; // Reset the position to the start
    }
}




static Void process_includes(Str* source)
{
    Char* src = str_to_char(source);
    Char* pos = src;

    while ((pos = strstr(pos, "#incl")) != NULL) {
        Char* end_line = strchr(pos, '\n');
        if (!end_line)
            break;

        // extract file name
        Char* file_start = pos + 6; // Start after "#incl "
        Char* file_end = end_line;
        Char* file_name = strndup(file_start, file_end - file_start);

        // read the content of the included file
        Char* included_content = read_file(file_name);
        if (!included_content) {
            PANIC("Failed to include file: %s", file_name);
        }

        // replace the #incl line with the content of the file
        Str* included_str = str_new(included_content);
        str_remove_range(source, pos - src, end_line + 1 - src);
        str_insert(source, pos - src, included_content);

        free(file_name);
        free(included_content);
        str_free(included_str);
    }
}

static Void remove_comments(Str* source)
{
    Char* src = str_to_char(source);
    Char* pos = src;
    Bool in_string = FALSE;
    Bool in_char = FALSE;

    while ((pos = strchr(pos, ':')) != NULL) {
        // Check if ':' is inside a string or a character
        Char* scan_pos = src;
        while (scan_pos < pos) {
            if (*scan_pos == '"' && (scan_pos == src || *(scan_pos - 1) != '\\')) {
                in_string = !in_string; // Toggle in_string state
            } else if (*scan_pos == '\'' && (scan_pos == src || *(scan_pos - 1) != '\\')) {
                in_char = !in_char; // Toggle in_char state
            }
            scan_pos++;
        }

        // If ':' is inside a string or character, skip it
        if (in_string || in_char) {
            pos++; // Move to the next character and continue
            continue;
        }

        // ':' is not inside a string or character, so it is a comment
        Char* end_line = strchr(pos, '\n');
        if (!end_line) {
            str_remove_range(source, pos - src, source->length); // Remove from ':' to end of string
            break;
        }
        else {
            str_remove_range(source, pos - src, end_line - src); // Remove the comment
        }
    }
}

static Void process_annotations(Str* source)
{
    Char* src = str_to_char(source);
    Char* pos = src;
    Bool in_string = FALSE;
    Bool in_char = FALSE;

    while ((pos = strchr(pos, '!')) != NULL) {
        // Check if '!' is inside a string, character, or is part of '!='
        Char* scan_pos = src;
        while (scan_pos < pos) {
            if (*scan_pos == '"' && (scan_pos == src || *(scan_pos - 1) != '\\')) {
                in_string = !in_string; // Toggle in_string state
            } else if (*scan_pos == '\'' && (scan_pos == src || *(scan_pos - 1) != '\\')) {
                in_char = !in_char; // Toggle in_char state
            }
            scan_pos++;
        }

        // If '!' is inside a string, character, or followed by '=', skip it
        if (in_string || in_char || (*(pos + 1) == '=')) {
            pos++; // Move to the next character and continue
            continue;
        }

        // Process the annotation
        if (pos[1] == ' ') { // skip the entire line if ! is followed by a space
            Char* end_line = strchr(pos, '\n');
            if (!end_line) {
                str_remove_range(source, pos - src, source->length);
                break;
            } else {
                str_remove_range(source, pos - src, end_line + 1 - src);
            }
        } else { // skip the word until the next space
            Char* space = strchr(pos, ' ');
            if (!space) {
                str_remove_range(source, pos - src, source->length);
                break;
            } else {
                str_remove_range(source, pos - src, space - src + 1);
            }
        }
    }
}

// main preprocessing function
Str* preprocess_file(const Char* filename, DictStr* macro_dict)
{
    Char* raw_content = read_file(filename);
    if (!raw_content) {
        PANIC("Failed to read file: %s", filename);
    }

    Str* source = str_new(raw_content);
    free(raw_content);

    remove_comments(source);
    process_annotations(source);

    process_includes(source);                   // 1
    collect_macros(source, macro_dict);
    process_conditionals(source, macro_dict);   // 3
    replace_macros(source, macro_dict);         // 2

    // Convert characters and strings to ASCII
    convert_chars_to_ascii(source);
    convert_string_literals_to_ascii_array(source);

    remove_comments(source);
    process_annotations(source);

    dictstr_free(macro_dict);

    return source;
}

Void write_preprocessed_file(const Char* base_dir, const Char* filename, const Str* processed_content)
{
    Str* output_file = str_new(base_dir);
    str_cat(output_file, "/");
    str_cat(output_file, filename);
    write_file(str_to_char(output_file), str_to_char(processed_content));
    str_free(output_file);
}
