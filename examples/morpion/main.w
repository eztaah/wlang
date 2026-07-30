#def BOARD_SIZE 3

!void clear_board(!ascii& <64> board)
{
    !int <64> i = 0;
    loop {
        if (i == BOARD_SIZE * BOARD_SIZE) {
            break;
        }
        ^(board + i*8) = 32;  : Remplir le plateau avec des espaces (ASCII 32)
        i = i + 1;
    }
}

!void print_board(!ascii& <64> board)
{
    !ascii <64> newline[2] = "\n";
    !int <64> i = 0;

    loop {
        print_number(i + 1);
        !ascii <64> pipe[2] = " |";
        !ascii <64> dash[2] = "-";
        !ascii <64> space[2] = " ";
        print_str_array(pipe, 2);
        print_str_array(space, 2);
        _print_digit(^(board + i*8));
        print_str_array(pipe, 2);
        if ((i + 1) % BOARD_SIZE == 0) {
            print_str_array(newline, 2);
            if (i < BOARD_SIZE * (BOARD_SIZE - 1)) {
                !ascii <64> row_separator[12] = "---------\n";
                print_str_array(row_separator, 12);
            }
        }
        i = i + 1;
    }
    print_str_array(newline, 2);
}

!void get_player_move(!ascii <64> current_player, !ascii& <64> board)
{
    !int <64> move;
    !int <64> valid_move = 0;
    !ascii <64> prompt[32] = "Enter your move (1-9): ";
    !ascii <64> invalid_move[25] = "Invalid move. Try again.\n";
    
    loop {
        if (valid_move == 1) {
            break;
        }

        print_str_array(prompt, 32);
        !void& <64> input_str = input();
        move = ascii_array_to_int(input_str, 64);

        free(input_str, 64);

        move = move - 1;  : Les positions de 1 à 9 correspondent aux index 0 à 8

        if (move >= 0 && move < BOARD_SIZE * BOARD_SIZE && ^(board + move*8) == 32) {
            ^(board + move*8) = current_player;
            valid_move = 1;
        } else {
            print_str_array(invalid_move, 25);
        }
    }
}

!int <64> check_winner(!ascii& <64> board)
{
    !ascii <64> current;
    !int <64> i = 0;

    : Vérifier les lignes
    loop {
        current = ^(board + i*8);
        if (current != 32 && current == ^(board + (i + 1)*8) && current == ^(board + (i + 2)*8)) {
            ret current;
        }
        i = i + BOARD_SIZE;
        if (i == BOARD_SIZE * BOARD_SIZE) {
            break;
        }
    }

    i = 0;

    : Vérifier les colonnes
    loop {
        current = ^(board + i*8);
        if (current != 32 && current == ^(board + (i + BOARD_SIZE)*8) && current == ^(board + (i + 2*BOARD_SIZE)*8)) {
            ret current;
        }
        i = i + 1;
        if (i == BOARD_SIZE) {
            break;
        }
    }

    : Vérifier la diagonale principale
    current = ^(board + 0*8);
    if (current != 32 && current == ^(board + 4*8) && current == ^(board + 8*8)) {
        ret current;
    }

    : Vérifier la diagonale secondaire
    current = ^(board + 2*8);
    if (current != 32 && current == ^(board + 4*8) && current == ^(board + 6*8)) {
        ret current;
    }

    ret 0;  : Pas de gagnant
}

!int <64> is_draw(!ascii& <64> board)
{
    !int <64> i = 0;

    loop {
        if (^(board + i*8) == 32) {
            ret 0;  : Encore des cases disponibles
        }
        i = i + 1;
        if (i == BOARD_SIZE * BOARD_SIZE) {
            break;
        }
    }

    ret 1;  : Toutes les cases sont remplies
}

glb !void game()
{
    !ascii <64> board[BOARD_SIZE * BOARD_SIZE * 8];  : Plateau de 3x3
    !ascii <64> current_player = 88;  : Le joueur 1 commence avec 'X' (ASCII 88)
    !ascii <64> winner;
    !ascii <64> draw_msg[20] = "It's a draw!\n";
    !ascii <64> win_msg[22] = "Player X wins!\n";
    
    clear_board(board);
    
    loop {
        print_board(board);
        get_player_move(current_player, board);
        
        winner = check_winner(board);
        if (winner != 0) {
            print_board(board);
            if (winner == 88) {  : Vérifier si le gagnant est 'X'
                print_str_array(win_msg, 22);
            } else {  : Sinon, c'est 'O' qui gagne
                win_msg[7] = 79;  : Modifier le message pour 'O'
                print_str_array(win_msg, 22);
            }
            break;
        }

        if (is_draw(board)) {
            print_board(board);
            print_str_array(draw_msg, 20);
            break;
        }

        if (current_player == 88) {
            current_player = 79;  : Changer de joueur (de 'X' à 'O')
        } else {
            current_player = 88;  : Changer de joueur (de 'O' à 'X')
        }
    }
}

!int <64> main()
{
    game();
    ret 0;
}
