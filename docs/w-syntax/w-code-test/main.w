%import print(void): void %from ./wlib/io/print.w
%import random(cst min: i32, cst max: i32): i32 %from ./wlib/math/random.while

%import display_grid(void): void %from ./morpion.w

fun main(void): i32 {
    srand(time(NULL))  // Seed the random number generator
    initialiserGrille()

    var tour: i32 = 0  // 0 for player, 1 for IA
    var etatJeu: i32 = 0  // 0 for game in progress, 1 for won, -1 for draw

    display_grid()
    while ((etatJeu = verifierGagnant()) == 0) {
        if (tour == 0) {
            tourJoueur()
            tour = 1
        } else {
            tourIA()
            tour = 0
            display_grid()
        }
    }

    display_grid()
    if (etatJeu == 1) {
        print("Fin du jeu ! Le %s gagne !\n", tour == 1 ? "joueur" : "IA")
    } else {
        print("Match nul !\n")
    }

    output 0
}
