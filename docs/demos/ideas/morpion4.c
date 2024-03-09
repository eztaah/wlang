#include <stdio.h>
#include <stdlib.h>

/*
    00 : case vide
    01 : X (joueur 1)
    10 : O (joueur 2)
*/

int main() {
    // Initialiser la grille
    int grille = 0b000000000000000000;

    // Afficher la grille initiale
    for (int i = 0; i < 9; i++) {
        printf("[ ]");
        if (i % 3 == 2) {
            printf("\n");
        }
    }

    int tour = 0; // 0 -> joueur 1, 1 -> joueur 2
    int etatJeu = 0;

    while (1) {
        etatJeu = 0;

        // affiche la grille 
        for (int i = 31; i >= 0; i--) {
            printf("%d", (grille >> i) & 1);
            if (i % 2 == 0) printf(" ");
        }
        printf("\n");

        // on regarde si quelqu'un à gagné
        for (int i = 0; i < 3; i++) {
            // lignes
            int ligne = (grille >> (2*i*3)) & 0b000000000000111111; // extrait les 6 bits représentant une ligne
            if ((ligne == 0b000000000000010101) || (ligne == 0b000000000000101010)) {
                etatJeu = 1;
            }
            // colonnes
            int colonne = ((grille >> (2*i)) & 0b000001) | ((grille >> (2*i + 6)) & 0b000100) | ((grille >> (2*i + 12)) & 0b010000);
            if ((colonne == 0b010101) || (colonne == 0b101010)) {
                etatJeu = 1;
            }
        }
        // diagonales
        int diag1 = (grille & 0b000001) | ((grille >> 8) & 0b000100) | ((grille >> 16) & 0b010000);
        int diag2 = ((grille >> 4) & 0b000001) | ((grille >> 8) & 0b000100) | ((grille >> 12) & 0b010000);
        if ((diag1 == 0b010101) || (diag1 == 0b101010) || (diag2 == 0b010101) || (diag2 == 0b101010)) {
            etatJeu = 1;
        }

        // Verifie si la grille est pleine
        int grillePleine = 1;
        for (int i = 0; i < 9; i++) {
            if (((grille >> (2 * i)) & 0b11) == 0) {
                grillePleine = 0;
                break;
            }
        }

        // Si la grille est pleine, match nul
        if (grillePleine && etatJeu == 0) {
            etatJeu = -1;
        }
        // Vérifie si le jeu est terminé
        if (etatJeu != 0) {
            break; 
        }

        // Tour du joueur 1 ou joueur 2
        int x = -1, y = -1;
        while (x < 0 || x > 2 || y < 0 || y > 2 || (((grille >> (2*(x*3+y))) & 3) != 0)) {
            printf("Joueur %d, entrez votre coup (ligne colonne): ", tour + 1);
            scanf("%d %d", &x, &y);
            x--; y--;
        }

        // mise à jour de la grille
        grille |= ((tour + 1) << (2*(x*3+y)));

        // Changement de tour
        tour = 1 - tour; 

        // Afficher la grille à chaque tour
        for (int i = 0; i < 9; i++) {
            char c;
            if (((grille >> (2*i)) & 0b000000000000000011) == 0b000000000000000000) {
                c = ' ';
            }
            else {
                if (((grille >> (2*i)) & 0b000000000000000011) == 0b000000000000000001) {
                    c = 'X';
                }
                else {
                    c = 'O';
                }
            }
            printf("[%c]", c);
            if (i % 3 == 2) {
                printf("\n");
            }
        }
    }

    // Afficher le résultat final
    if (etatJeu == 1) {
        if (tour == 0) {
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



// // print ligne
// for (int i = 31; i >= 0; i--) {
//     printf("%d", (ligne >> i) & 1);
// }
// printf("\n");