#include <stdio.h>
#include <dirent.h>

int main(int argc, char *argv[]) 
{
// struct dirent
//   {
//     __ino_t d_ino;
//     __off_t d_off;
//     unsigned short int d_reclen;
//     unsigned char d_type;
//     char d_name[256];		/* We must not include limits.h! */
//   };
    struct dirent* entry;
    DIR *dp;

    char* directory = ".";
    dp = opendir(directory);

    if (dp == NULL) {
        perror("opendir");  // Affiche une erreur si le répertoire ne peut pas être ouvert
        return 1;
    }

    while ((entry = readdir(dp))) {
        printf("%s\n", entry->d_name);  // Affiche chaque entrée du répertoire
    }

    closedir(dp);
    return 0;
}
