"""
tour :  0 -> joueur 1
        1 -> joueur 2
"""

# Initialisation
var i: i64 = 0
var grille: i64 = 0
var tour: i64 = 0
var etat_jeu: i64 = 0

# Afficher la grille initiale
i = 0
while (i < 9) {
    print("[ ]")
    if (i % 3 == 2) {
        print("\n")
    }
    i = i + 1
}

# Game loop
while (1) {

    # Demande à l'utilisateur de choisir la position de son coup
    var x: i64 = -1
    var y: i64 = -1
    while (x < 0 || x > 2 || y < 0 || y > 2 || (((grille >> (2*(x*3+y))) & 3) != 0)) {
        print("Joueur ", tour + 1, " entrez votre coup (ligne colonne): ")
        x = input()
        y = input()
        x = x - 1
        y = y - 1
    }

    # mise à jour de la grille
    grille = grille | ((tour + 1) << (2*(x*3+y)))

    # on regarde si quelqu'un à gagné
    i = 0
    while (i < 3) {
        # lignes
        cst ligne: i64 = (grille >> (2*i*3)) & 63        # extrait les 6 bits représentant une ligne
        if ((ligne == 21) || (ligne == 42)) {
            etat_jeu = 1
        }
        # colonnes
        cst colonne: i64 = ((grille >> (2 * i)) & 3) | ((grille >> (2 * i + 6)) & 3) << 2 | ((grille >> (2 * i + 12)) & 3) << 4
        if ((colonne == 21) || (colonne == 42)) {
            etat_jeu = 1
        }
        i = i + 1
    }
    # diagonales
    cst diag1: i64 = ((grille >> (2 * 0)) & 3) | ((grille >> (2 * 4)) & 3) << 2 | ((grille >> (2 * 8)) & 3) << 4
    cst diag2: i64 = ((grille >> (2 * 2)) & 3) | ((grille >> (2 * 4)) & 3) << 2 | ((grille >> (2 * 6)) & 3) << 4
    if ((diag1 == 21) || (diag1 == 42) || (diag2 == 21) || (diag2 == 42)) {
        etat_jeu = 1
    }

    # Afficher la grille à chaque tour
    i = 0
    while (i < 9) {
        var c: i64 = -1
        if (((grille >> (2*i)) & 3) == 0) {
            c = 0
        }
        else {
            if (((grille >> (2*i)) & 3) == 1) {
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

    # Verifie si la grille est pleine
    var grillePleine: i64 = 1
    i = 0
    while (i < 9) {
        if (((grille >> (2 * i)) & 3) == 0) {
            grillePleine = 0
        }
        i = i + 1
    }

    # Si la grille est pleine, match nul
    if (grillePleine == 1 && etat_jeu == 0) {
        etat_jeu = -1
    }

    # Vérifie si le jeu est terminé
    if (etat_jeu != 0) {
        # Afficher le résultat final
        if (etat_jeu == 1) {
            if (tour == 1) {
                print("Fin du jeu ! Le joueur ", 2, " gagne !\n")
            }
            else {
                print("Fin du jeu ! Le joueur ", 1, " gagne !\n")
            }
        } 
        else {
            print("Match nul !\n")
        }
        exit()
    }

    # Changement de tour
    tour = 1 - tour
}
