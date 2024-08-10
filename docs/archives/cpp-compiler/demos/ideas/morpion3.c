#include <stdio.h>
#include <stdlib.h>


int main() {
    char grille[3][3];

    // Initialiser la grille
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            grille[i][j] = ' ';
        }
    }

    // Afficher la grille initiale
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("[%c]", grille[i][j]);
        }
        printf("\n");
    }

    int tour = 0; // 0 pour le joueur 1, 1 pour le joueur 2
    int etatJeu = 0; // 0 pour jeu en cours, 1 pour gagné, -1 pour match nul

    while (1) {
        // Vérification des lignes, des colonnes et des diagonales
        etatJeu = 0;
        for (int i = 0; i < 3; i++) {
            if (grille[i][0] == grille[i][1] && grille[i][1] == grille[i][2] && grille[i][0] != ' ') {
                etatJeu = 1;
            }
            if (grille[0][i] == grille[1][i] && grille[1][i] == grille[2][i] && grille[0][i] != ' ') {
                etatJeu = 1;
            }
        }
        if (grille[0][0] == grille[1][1] && grille[1][1] == grille[2][2] && grille[0][0] != ' ') {
            etatJeu = 1;
        } 
        if (grille[0][2] == grille[1][1] && grille[1][1] == grille[2][0] && grille[0][2] != ' ') {
            etatJeu = 1;
        }

        // Vérification si la grille est pleine
        int grillePleine = 1;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (grille[i][j] == ' ') {
                    grillePleine = 0;
                    break;
                }
            }
            if (!grillePleine) {
                break;
            }
        }
        if (grillePleine) {
            etatJeu = -1;
        }

        if (etatJeu != 0) {
            break; // Sortie si le jeu est terminé
        }

        // Tour du joueur 1 ou joueur 2
        int x = -1, y = -1;
        while (x < 0 || x > 2 || y < 0 || y > 2 || grille[x][y] != ' ') {
            printf("Joueur %d, entrez votre coup (ligne colonne): ", tour + 1);
            scanf("%d %d", &x, &y);
            x--; y--; // Ajustement pour l'indexation à base de zéro
        }

        grille[x][y] = (tour == 0) ? 'X' : 'O';
        tour = 1 - tour; // Changement de tour

        // Afficher la grille à chaque tour
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                printf("[%c]", grille[i][j]);
            }
            printf("\n");
        }
    }

    // Afficher le résultat final
    if (etatJeu == 1) {
        printf("Fin du jeu ! Le joueur %d gagne !\n", (tour == 0) ? 2 : 1);
    } 
    else {
        printf("Match nul !\n");
    }

    return 0;
}
