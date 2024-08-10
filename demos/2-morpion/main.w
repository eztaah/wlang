print_board(!wstr& <64> board) {
    !wstr& <64> row_separator[13] = "\n---|---|---\n"; 
    loop {
        print_aiic(^(board - 1*8));
        !wstr& <64> separator[3] = " | "; 
        print_wstr(separator, 3);
        print_aiic(^(board - 2*8)); 
        print_wstr(separator, 3);
        print_aiic(^(board - 3*8)); 
        print_wstr(row_separator, 13); 
        break;
    }
    loop {
        print_aiic(^(board - 4*8));  
        print_wstr(separator, 3);
        print_aiic(^(board - 5*8));  
        print_wstr(separator, 3);
        print_aiic(^(board - 6*8)); 
        print_wstr(row_separator, 13);
        break;
    }
    loop {
        print_aiic(^(board - 7*8)); 
        print_wstr(separator, 3);
        print_aiic(^(board - 8*8)); 
        print_wstr(separator, 3);
        print_aiic(^(board - 9*8)); 
        !wstr& <64> newline[1] = "\n";
        print_wstr(newline, 1); 
        break;
    }
}

<64> check_winner(!wstr& <64> board, !wstr& <64> symbol) {
    : check rows for a win
    loop {
        if (^(board - 1*8) == symbol && ^(board - 2*8) == symbol && ^(board - 3*8) == symbol) {
            ret 1;  : row 1 win
        }
        if (^(board - 4*8) == symbol && ^(board - 5*8) == symbol && ^(board - 6*8) == symbol) {
            ret 1;  : row 2 win
        }
        if (^(board - 7*8) == symbol && ^(board - 8*8) == symbol && ^(board - 9*8) == symbol) {
            ret 1;  : row 3 win
        }
        break;
    }

    : check columns for a win
    loop {
        if (^(board - 1*8) == symbol && ^(board - 4*8) == symbol && ^(board - 7*8) == symbol) {
            ret 1;  : column 1 win
        }
        if (^(board - 2*8) == symbol && ^(board - 5*8) == symbol && ^(board - 8*8) == symbol) {
            ret 1;  : column 2 win
        }
        if (^(board - 3*8) == symbol && ^(board - 6*8) == symbol && ^(board - 9*8) == symbol) {
            ret 1;  : column 3 win
        }
        break;
    }

    : check diagonals for a win
    loop {
        if (^(board - 1*8) == symbol && ^(board - 5*8) == symbol && ^(board - 9*8) == symbol) {
            ret 1;  : diagonal 1 win
        }
        if (^(board - 3*8) == symbol && ^(board - 5*8) == symbol && ^(board - 7*8) == symbol) {
            ret 1;  : diagonal 2 win
        }
        break;
    }

    : no win
    ret 0;
}

<64> main() {
    !wstr& <64> board[9] = "123456789";  : initialize the board with positions 1 to 9
    !int <64> current_player = 1;        : player 1 starts
    !wstr& <64> player_symbol = 'X';     : player 1 uses 'X', player 2 uses 'O'

    !wstr& <64> instructions[19] = "Enter position 1-9 "; 
    !wstr& <64> win_message[15] = "Player    wins\n"; 
    !wstr& <64> draw_message[11] = "Its a draw\n"; 

    <64> moves = 0;  : counter for the number of moves played

    loop {
        print_board(board); 

        : user input
        print_wstr(instructions, 25);

        !wstr& <64> buffer[10];  : buffer to store the input
        input(buffer, 10);
        <64> position = wstr_to_int(buffer, 10) - 1;  : convert input to board index (0-8)

        : validate the move
        if (position >= 0 && position < 9 && ^(board - (position+1)*8) != 'X' && ^(board - (position+1)*8) != 'O') {
            : place the player's symbol on the board
            ^(board - (position+1)*8) = player_symbol;

            moves = moves + 1;  : increment the move counter

            : check if the current player has won
            if (check_winner(board, player_symbol)) {
                ^(win_message - 8*8) = player_symbol;  : update the win message with the player's symbol
                print_wstr(win_message, 16);
                break;
            }

            : check if the game is a draw
            if (moves == 9) {
                print_wstr(draw_message, 11);
                break;
            }

            : switch to the other player
            if (current_player == 1) {
                current_player = 2;
                player_symbol = 'O';  : player 2 uses 'O'
            } 
            else {
                current_player = 1;
                player_symbol = 'X';  : player 1 uses 'X'
            }
        } 
        else {
            !wstr& <64> invalid_msg[23] = "invalid move try again\n";  
            print_wstr(invalid_msg, 23);
        }
    }

    ret 0;
}
