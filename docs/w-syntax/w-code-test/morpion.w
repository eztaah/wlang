%import print %from wlib

%replace VIDE -> ' '



fun initialize_grid(void): void {
    for (i in 0..2) {
        for (j in 0..2) {
            grille[i][j] = VIDE
        }
    }
    output void
}

fun display_grid(void): void {
    for (i in 0..2) {
        for (j in 0..2) {
            print("[%c]", grille[i][j])
        }
        print("\n")
    }
    output void
}


fun check_winner(void): i32 {
    for (i in 0..2) {
        if (grille[i][0] == grille[i][1] && grille[i][1] == grille[i][2] && grille[i][0] != VIDE) {
            output 1
        }
        if (grille[0][i] == grille[1][i] && grille[1][i] == grille[2][i] && grille[0][i] != VIDE) {
            output 1
        }
    }
    if (grille[0][0] == grille[1][1] && grille[1][1] == grille[2][2] && grille[0][0] != VIDE) {
        output 1
    }
    if (grille[0][2] == grille[1][1] && grille[1][1] == grille[2][0] && grille[0][2] != VIDE) {
        output 1
    }
    for (i in 0..2) {
        for (j in 0..2) {
            if (grille[i][j] == VIDE) {
                output 0
            }
        }
    }
    output -1  // Match nul
}

fun player_turn(void): void {
    var x: i32
    var y: i32
    do {
        print("Entrez votre coup (ligne colonne): ")
        read(x, y)
        x -= 1
        y -= 1
    } while (x < 0 || x > 2 || y < 0 || y > 2 || grille[x][y] != VIDE)

    grille[x][y] = JOUEUR
}

fun tourIA(void): void {
    var x: i32
    var y: i32
    do {
        x = rand() % 3
        y = rand() % 3
    } while (grille[x][y] != VIDE)

    grille[x][y] = IA
}