# initialization
var i: i64 = 0
var grid: i64 = 0
var turn: i64 = 0
var game_state: i64 = 0

# draw an empty grid
i = 0
while (i < 9) {
    print("[ ]")
    if (i % 3 == 2) {
        print("\n")
    }
    i = i + 1
}

# game loop
while (1) {

    # ask the user to choose a cell
    var x: i64 = -1
    var y: i64 = -1
    while (x < 0 || x > 2 || y < 0 || y > 2 || (((grid >> (2*(x*3+y))) & 0b11) != 0)) {
        print("Player ", turn + 1, " enter your move (row column): ")
        x = input()
        y = input()
        x = x - 1
        y = y - 1
    }

    # update the grid
    grid = grid | ((turn + 1) << (2*(x*3+y)))

    # checking if someone has won
    i = 0
    while (i < 3) {
        # row
        cst ligne: i64 = (grid >> (2*i*3)) & 0b111111
        if ((ligne == 0b010101) || (ligne == 0b101010)) {
            game_state = 1
        }
        # columns
        cst colonne: i64 = ((grid >> (2 * i)) & 0b11) | ((grid >> (2 * i + 6)) & 0b11) << 2 | ((grid >> (2 * i + 12)) & 0b11) << 4
        if ((colonne == 0b010101) || (colonne == 0b101010)) {
            game_state = 1
        }
        i = i + 1
    }
    # diagonals
    cst diag1: i64 = ((grid >> (2 * 0)) & 0b11) | ((grid >> (2 * 4)) & 0b11) << 2 | ((grid >> (2 * 8)) & 0b11) << 4
    cst diag2: i64 = ((grid >> (2 * 2)) & 0b11) | ((grid >> (2 * 4)) & 0b11) << 2 | ((grid >> (2 * 6)) & 0b11) << 4
    if ((diag1 == 0b010101) || (diag1 == 0b101010) || (diag2 == 0b010101) || (diag2 == 0b101010)) {
        game_state = 1
    }

    # draw the grid
    i = 0
    while (i < 9) {
        var c: i64 = -1
        if (((grid >> (2*i)) & 0b11) == 0) {
            c = 0
        }
        else {
            if (((grid >> (2*i)) & 0b11) == 1) {
                c = 1
            }
            else {
                c = 2
            }
        }
        if (c == 0) {
            print("[ ]")
        }
        if (c == 1) {
            print("[X]")
        }
        if (c == 2) {
            print("[O]")
        }
        if (i % 3 == 2) {
            print("\n")
        }
        i = i + 1
    }

    # check if the grid is full
    var grid_is_full: i64 = 1
    i = 0
    while (i < 9) {
        if (((grid >> (2 * i)) & 0b11) == 0) {
            grid_is_full = 0
        }
        i = i + 1
    }

    # if the grid is full, it's a draw
    if (grid_is_full == 1 && game_state == 0) {
        game_state = -1
    }

    # check if the game is over
    if (game_state != 0) {
        # display result
        if (game_state == 1) {
            if (turn == 1) {
                print("End of game! Player ", 2, " wins!\n")
            }
            else {
                print("End of game! Player ", 1, " wins!\n")
            }
        } 
        else {
            print("It's a draw!\n");
        }
        exit()
    }

    # change turn
    turn = 1 - turn
}
