#include <stdio.h>
#include <stdarg.h>     // fournis les macros pour acceder à la liste d'arg d'une fonction variadique

void print(const char *format, ...) {
    va_list args;               // pour stocker des informations
    va_start(args, format);     // initialise args pour récupeer les argument supplementaires apres le paramètre format
    vprintf(format, args);      // variante de printf qui acceptes va_list comme argument
    va_end(args);               // nettoie
}