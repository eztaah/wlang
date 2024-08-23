!int <64> main() 
{
    !ascii <64> directory[2] = ".\0";
    !int <64> dir_len = 1;

    : Allouer de la mémoire pour le pointeur DIR* (par exemple 8 octets)
    !void* <64> dp = @malloc(8);

    : Ouvrir le répertoire avec opendir
    !ascii& <64> directory_char_ptr = convert_ascii_array_to_char_ptr(directory, 2);
    dp = @opendir(directory_char_ptr);

    : Vérifier si le répertoire est ouvert avec succès
    if (dp == 0) {
        : Affiche un message d'erreur
        !ascii <64> error_msg[14] = "Error opening\n";
        print_ascii_array(error_msg, 14);
        ret 1;
    }

    : Allouer de la mémoire pour le struct dirent
    : !void* <64> entry = @malloc(280);  : 256 + 4*8 (4 * 64 bits pour d_ino, d_off, d_reclen, d_type)

    : Lire les entrées du répertoire
    loop {
        <64> entry = @readdir(dp);
        
        : Si entry est nul, cela signifie que toutes les entrées ont été lues
        if (entry == 0) {
            break;
        }

        @puts(entry + 19);
    }

    : Fermer le répertoire avec closedir
    @closedir(dp);

    : Libérer la mémoire
    @free(entry, 280);

    ret 0;
}
