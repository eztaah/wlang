!int <64> main() 
{
    !ascii <64> directory[2] = ".\0"; : set directory to current directory

    : allocate memory for the DIR* pointer
    !void& <64> dp = malloc(8);

    : open the directory with opendir
    !cstr& <64> directory_cstr = wstr_to_cstr(directory, 2);
    dp = @opendir(directory_cstr);

    : check if the directory is opened successfully
    if (dp == 0) {
        !ascii <64> error_msg[14] = "Error opening\n";
        print_wstr(error_msg, 14);
        ret 1;
    }

    : allocate memory for struct dirent
    : !void* <64> entry = @malloc(280); // 256 + 4*8 (4 * 64 bits for d_ino, d_off, d_reclen, d_type)

    : read directory entries
    loop {
        <64> entry = @readdir(dp);
        
        : if entry is null, all entries have been read
        if (entry == 0) {
            break;
        }

        @puts(entry + 19); : print the name of the directory entry 
    }

    : close the directory
    @closedir(dp);

    @free(entry, 280);

    ret 0;
}
