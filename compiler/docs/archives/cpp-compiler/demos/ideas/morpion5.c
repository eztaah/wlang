#include <stdio.h>
#include <stdlib.h>

/*
    00 : case vide
    01 : X (joueur 1)
    10 : O (joueur 2)
*/

int main() {
    // Initialisation
    int i = 0;
    int grille = 0;
    int tour = 0;
    int etat_jeu = 0;

    // Afficher la grille initiale
    i = 0;
    while (i < 9) {
        printf("[ ]");
        if (i % 3 == 2) {
            printf("\n");
        }
        i = i + 1;
    }

    // Game loop
    while (1) {

        // Demande à l'utilisateur de choisir la position de son coup
        int x = -1;
        int y = -1;
        while (x < 0 || x > 2 || y < 0 || y > 2 || (((grille >> (2*(x*3+y))) & 0b11) != 0)) {
            printf("Joueur %d, entrez votre coup (ligne colonne): ", tour + 1);
            scanf("%d %d", &x, &y);
            x--; y--;
        }

        // mise à jour de la grille
        grille = grille | ((tour + 1) << (2*(x*3+y)));

        // on regarde si quelqu'un à gagné
        i = 0;
        while (i < 3) {
            // lignes
            int ligne = (grille >> (2*i*3)) & 0b111111; // extrait les 6 bits représentant une ligne
            if ((ligne == 0b010101) || (ligne == 0b101010)) {
                etat_jeu = 1;
            }
            // colonnes
            int colonne = ((grille >> (2 * i)) & 0b11) | ((grille >> (2 * i + 6)) & 0b11) << 2 | ((grille >> (2 * i + 12)) & 0b11) << 4;
            if ((colonne == 0b010101) || (colonne == 0b101010)) {
                etat_jeu = 1;
            }
            i = i + 1;
        }
        // diagonales
        int diag1 = ((grille >> (2 * 0)) & 0b11) | ((grille >> (2 * 4)) & 0b11) << 2 | ((grille >> (2 * 8)) & 0b11) << 4;
        int diag2 = ((grille >> (2 * 2)) & 0b11) | ((grille >> (2 * 4)) & 0b11) << 2 | ((grille >> (2 * 6)) & 0b11) << 4;
        if ((diag1 == 0b010101) || (diag1 == 0b101010) || (diag2 == 0b010101) || (diag2 == 0b101010)) {
            etat_jeu = 1;
        }

        // Afficher la grille à chaque tour
        i = 0;
        while (i < 9) {
            int c = -1;
            if (((grille >> (2*i)) & 0b11) == 0) {
                c = 0;
            }
            else {
                if (((grille >> (2*i)) & 0b11) == 1) {
                    c = 1;
                }
                else {
                    c = 2;
                }
            }
            if (c == 0) {
                printf("[ ]");
            }
            if (c == 1) {
                printf("[X]");
            }
            if (c == 2) {
                printf("[O]");
            }
            if (i % 3 == 2) {
                printf("\n");
            }
            i = i + 1;
        }

        // Verifie si la grille est pleine
        int grillePleine = 1;
        i = 0;
        while (i < 9) {
            if (((grille >> (2 * i)) & 0b11) == 0) {
                grillePleine = 0;
            }
            i = i + 1;
        }

        // Si la grille est pleine, match nul
        if (grillePleine == 1 && etat_jeu == 0) {
            etat_jeu = -1;
        }
        // Vérifie si le jeu est terminé
        if (etat_jeu != 0) {
            // Afficher le résultat final
            if (etat_jeu == 1) {
                if (tour == 1) {
                    printf("Fin du jeu ! Le joueur %d gagne !\n", 2);
                }
                else {
                    printf("Fin du jeu ! Le joueur %d gagne !\n", 1);
                }
            } 
            else {
                printf("Match nul !\n");
            }

            return 0;
        }

        // Changement de tour
        tour = 1 - tour; 
    }
}