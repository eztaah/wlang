#include "lib.h"
#include <stdlib.h> // malloc(), free()
#include <string.h> // strstr(), strchr(), strlen(), strcpy(), strcat()


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

    while ((pos = strchr(pos, ':')) != NULL) {
        Char* end_line = strchr(pos, '\n');
        if (!end_line) {
            str_remove_range(source, pos - src, source->length); // Remove from : to end of string
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

    while ((pos = strchr(pos, '!')) != NULL) {
        if (pos[1] == ' ') { // skip the entire line if ! is followed by a space
            Char* end_line = strchr(pos, '\n');
            if (!end_line) {
                str_remove_range(source, pos - src, source->length);
                break;
            }
            else {
                str_remove_range(source, pos - src, end_line + 1 - src);
            }
        }
        else { // skip the word until the next space
            Char* space = strchr(pos, ' ');
            if (!space) {
                str_remove_range(source, pos - src, source->length);
                break;
            }
            else {
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
