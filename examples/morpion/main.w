!int <64> main() {
    !ascii <64> board[9] = "123456789";  : Initialise la grille avec des chiffres de 1 a 9 pour chaque position
    !int <64> current_player = 1;  : Joueur 1 commence
    !ascii <64> player_symbol = 'X';  : Le joueur 1 utilise X, le joueur 2 utilise O

    !ascii <64> instructions[19] = "Enter position 1-9 ";
    !ascii <64> win_message[15] = "Player    wins\n";
    !ascii <64> draw_message[11] = "Its a draw\n";

    !int <64> moves = 0;  : Compte le nombre de coups joues

    loop {
        print_board(board);  : Affiche la grille

        : Saisie de l'utilisateur
        print_ascii_array(instructions, 25);

        !ascii& <64> buffer[10];
        input(buffer, 10);
        !int <64> position = ascii_array_to_int(buffer, 10) - 1;

        : Verifie si la position est valide
        if (position >= 0 && position < 9 && ^(board - (position+1)*8) != 'X' && ^(board - (position+1)*8) != 'O') {
            : Place le symbole sur la grille
            ^(board - (position+1)*8) = player_symbol;

            moves = moves + 1;

            : Verifie si le joueur courant a gagne
            if (check_winner(board, player_symbol)) {
                ^(win_message - 8*8) = player_symbol;  : Mise a jour du message de victoire avec le symbole du joueur
                print_ascii_array(win_message, 16);
                break;
            }

            : Verifie si la partie est un match nul
            if (moves == 9) {
                print_ascii_array(draw_message, 11);
                break;
            }

            : Change de joueur
            if (current_player == 1) {
                current_player = 2;
                player_symbol = 'O';  : Joueur 2 utilise O
            } else {
                current_player = 1;
                player_symbol = 'X';  : Joueur 1 utilise X
            }
        } else {
            !ascii <64> invalid_msg[22] = "Invalid move try again\n";
            print_ascii_array(invalid_msg, 22);
        }
    }

    ret 0;
}

: Fonction pour afficher la grille
glb !void print_board(!ascii <64> board) {
    !ascii <64> row_separator[13] = "\n---|---|---\n";
    loop {
        print_ascii(^(board - 1*8));
        !ascii <64> separator[3] = " | ";
        print_ascii_array(separator, 3);
        print_ascii(^(board - 2*8));
        print_ascii_array(separator, 3);
        print_ascii(^(board - 3*8));
        print_ascii_array(row_separator, 13);
        break;
    }
    loop {
        print_ascii(^(board - 4*8));
        print_ascii_array(separator, 3);
        print_ascii(^(board - 5*8));
        print_ascii_array(separator, 3);
        print_ascii(^(board - 6*8));
        print_ascii_array(row_separator, 13);
        break;
    }
    loop {
        print_ascii(^(board - 7*8));
        print_ascii_array(separator, 3);
        print_ascii(^(board - 8*8));
        print_ascii_array(separator, 3);
        print_ascii(^(board - 9*8));
        !ascii <64> newline[1] = "\n";
        print_ascii_array(newline, 1);
        break;
    }
}

: Fonction pour verifier si un joueur a gagne
glb !int <64> check_winner(!ascii <64> board, !ascii <64> symbol) {
    : Verifie les lignes
    loop {
        if (^(board - 1*8) == symbol && ^(board - 2*8) == symbol && ^(board - 3*8) == symbol) {
            ret 1;
        }
        if (^(board - 4*8) == symbol && ^(board - 5*8) == symbol && ^(board - 6*8) == symbol) {
            ret 1;
        }
        if (^(board - 7*8) == symbol && ^(board - 8*8) == symbol && ^(board - 9*8) == symbol) {
            ret 1;
        }
        break;
    }

    : Verifie les colonnes
    loop {
        if (^(board - 1*8) == symbol && ^(board - 4*8) == symbol && ^(board - 7*8) == symbol) {
            ret 1;
        }
        if (^(board - 2*8) == symbol && ^(board - 5*8) == symbol && ^(board - 8*8) == symbol) {
            ret 1;
        }
        if (^(board - 3*8) == symbol && ^(board - 6*8) == symbol && ^(board - 9*8) == symbol) {
            ret 1;
        }
        break;
    }

    : Verifie les diagonales
    loop {
        if (^(board - 1*8) == symbol && ^(board - 5*8) == symbol && ^(board - 9*8) == symbol) {
            ret 1;
        }
        if (^(board - 3*8) == symbol && ^(board - 5*8) == symbol && ^(board - 7*8) == symbol) {
            ret 1;
        }
        break;
    }

    : Pas de victoire
    ret 0;
}
