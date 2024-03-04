#include <stdio.h>

int main() {
    char grille[3][3] = {{' ', ' ', ' '}, {' ', ' ', ' '}, {' ', ' ', ' '}};
    int x, y, etatJeu = 0, tour = 0, i, j;

    while (etatJeu == 0) {
        if (tour == 0) {
            printf("Entrez votre coup (ligne colonne): ");
            scanf("%d %d", &x, &y);
            x--; y--;
            if (x >= 0 && x < 3 && y >= 0 && y < 3 && grille[x][y] == ' ') {
                grille[x][y] = 'X';
            }
        } else {
            int fait = 0;
            for (i = 0; i < 3 && !fait; i++) {
                for (j = 0; j < 3 && !fait; j++) {
                    if (grille[i][j] == ' ') {
                        grille[i][j] = 'O';
                        fait = 1;
                    }
                }
            }
        }

        int joueurGagne = 0, iaGagne = 0, plein = 1;
        for (i = 0; i < 3; i++) {
            if (grille[i][0] == grille[i][1] && grille[i][1] == grille[i][2] && grille[i][0] != ' ') {
                if (grille[i][0] == 'X') joueurGagne = 1; else iaGagne = 1;
            }
            if (grille[0][i] == grille[1][i] && grille[1][i] == grille[2][i] && grille[0][i] != ' ') {
                if (grille[0][i] == 'X') joueurGagne = 1; else iaGagne = 1;
            }
            for (j = 0; j < 3; j++) {
                if (grille[i][j] == ' ') plein = 0;
            }
        }
        if (grille[0][0] == grille[1][1] && grille[1][1] == grille[2][2] && grille[0][0] != ' ') {
            if (grille[0][0] == 'X') joueurGagne = 1; else iaGagne = 1;
        }
        if (grille[0][2] == grille[1][1] && grille[1][1] == grille[2][0] && grille[0][2] != ' ') {
            if (grille[0][2] == 'X') joueurGagne = 1; else iaGagne = 1;
        }
        if (joueurGagne || iaGagne || plein) etatJeu = 1;

        for (i = 0; i < 3; i++) {
            for (j = 0; j < 3; j++) {
                printf("[%c]", grille[i][j]);
            }
            printf("\n");
        }

        tour = tour == 0 ? 1 : 0;
    }

    if (joueurGagne) printf("Fin du jeu ! Le joueur gagne !\n");
    else if (iaGagne) printf("Fin du jeu ! L'IA gagne !\n");
    else printf("Match nul !\n");

    return 0;
}
