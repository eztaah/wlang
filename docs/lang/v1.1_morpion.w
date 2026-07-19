import print from wlib
import input from wlib
import rand from wlib
import srand from wlib
import time from wlib

replace UNREACHABLE() by panic(FILE, LINE, "unreachable")
replace GRID_LENGTH by 9

fun panic(cst file_path: &C8, cst line: I32, cst message: &C8): Void
{
    print("%s:%d: APPLICATION PANIC: %s\n", file_path, line, message)
    exit(1)
}

fun tile_to_char(cst tile: Tile): C8
{
    if (tile == 0) {
        return 'O'
    }
    elif (tile == 1) {
        return 'X'
    }
    elif (tile == -1) {
        return ' '
    }
    else { 
        UNREACHABLE() 
    }
}

fun init_grid(var grid: &I32): Void 
{
    for (i in 0..GRID_LENGTH) {
        grid[i] = -1
    }
}

fun display_grid(cst grid: &I32): Void 
{
    for (i in 0..3) {
        for (j in 0..3) {
            printf("[%c]", tile_to_char(grid[3*i + j]))
        }
        printf("\n")
    }
}

fun update_game_state(cst grid: &I32): I32
{
    # Vérification des lignes, des colonnes et des diagonales

    for (i in 0..3) {
        if (grid[3*i + 0] == grid[3*i + 1] && grid[3*i + 1] == grid[3*i + 2] && grid[3*i + 2] != -1) {
            return 1       # 1 for win
        }
        if (grid[0 + i] == grid[3*1 + i] && grid[3*1 + i] == grid[3*2 + i] && grid[3*0 + i] != -1) {
            return 1
        }
    }

    if (grid[3*0 + 0] == grid[3*1 + 1] && grid[3*1 + 1] == grid[3*2 + 2] && grid[3*0 + 0] != -1) { 
        return 1
    }
    if (grid[3*0 + 2] == grid[3*1 + 1] && grid[3*1 + 1] == grid[3*2 + 0] && grid[3*0 + 2] != -1) {
        return 1
    }

    # Vérifier si la grid est pleine
    for (i in 0..GRID_LENGTH) {
        if (grid[i] == -1) {
            return -1      # -1 for playing
        }
    }

    return 0       # 0 for draw
}

fun play_human_turn(grid: &I32): Void
{
    I32 x = -1
    I32 y = -1

    while (x < 0 || x > 2 || y < 0 || y > 2 || grid[3*x + y] != -1) {
        printf("Enter your move (row column): ")
        scanf("%d %d", &x, &y)
        # Reduce the number because the player gives a position between 1 and 3, but the game logic is between 0 and 2
        x--
        y--
    }

    grid[3*x + y] = 1      # 1 stand for a cross
}

fun play_ia_turn(grid: &I32): Void
{
    I32 x = -1
    I32 y = -1

    while (grid[3*x + y] != -1) {
        x = rand() % 3
        y = rand() % 3
    }

    grid[3*x + y] = 0
}

fun main(Void):  {
    srand(time())

    var grid: I32[GRID_LENGTH]
    var current_player: I32 = 1
    var state: I32 = -1

    init_grid(grid)
    display_grid(grid)

    while (state == -1) {
        if (current_player == 1) {
            play_human_turn(grid)
            current_player = 2
        } 
        else {
            play_ia_turn(grid);
            current_player = 1
            display_grid(grid)
        }
        state = update_game_state(grid)
    }

    # The game is either win of draw
    display_grid(grid);
    if (state == 1) {
        if (current_player == 1) {
            printf("End of game! AI wins!\n")
        } 
        else {
            printf("End of game! Human wins!\n")
        }
    } 
    elif (state == 0) {
        printf("It's a draw!\n")
    } 
    else { 
        UNREACHABLE()
    }

    return 0
}
