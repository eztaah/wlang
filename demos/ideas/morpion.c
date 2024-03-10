#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define VIDE ' '
#define JOUEUR 'X'
#define IA 'O'

char grille[3][3];

void initialiserGrille() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            grille[i][j] = VIDE;
        }
    }
}

void afficherGrille() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            printf("[%c]", grille[i][j]);
        }
        printf("\n");
    }
}

int verifierGagnant() {
    // Vérification des lignes, des colonnes et des diagonales
    for (int i = 0; i < 3; i++) {
        if (grille[i][0] == grille[i][1] && grille[i][1] == grille[i][2] && grille[i][0] != VIDE) return 1;
        if (grille[0][i] == grille[1][i] && grille[1][i] == grille[2][i] && grille[0][i] != VIDE) return 1;
    }
    if (grille[0][0] == grille[1][1] && grille[1][1] == grille[2][2] && grille[0][0] != VIDE) return 1;
    if (grille[0][2] == grille[1][1] && grille[1][1] == grille[2][0] && grille[0][2] != VIDE) return 1;

    // Vérification si la grille est pleine
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (grille[i][j] == VIDE) return 0;
        }
    }
    return -1; // Match nul
}

void tourJoueur() {
    int x, y;
    do {
        printf("Entrez votre coup (ligne colonne): ");
        scanf("%d %d", &x, &y);
        x--; y--; // Ajustement pour l'indexation à base de zéro
    } while (x < 0 || x > 2 || y < 0 || y > 2 || grille[x][y] != VIDE);

    grille[x][y] = JOUEUR;
}

void tourIA() {
    int x, y;
    do {
        x = rand() % 3;
        y = rand() % 3;
    } while (grille[x][y] != VIDE);

    grille[x][y] = IA;
}

int main() {
    srand(time(NULL)); // Initialisation du générateur de nombres aléatoires
    initialiserGrille();

    int tour = 0; // 0 pour le joueur, 1 pour l'IA
    int etatJeu = 0; // 0 pour jeu en cours, 1 pour gagné, -1 pour match nul
    
    afficherGrille();
    while ((etatJeu = verifierGagnant()) == 0) {
        if (tour == 0) {
            tourJoueur();
            tour = 1;
        } else {
            tourIA();
            tour = 0;
            afficherGrille();
        }
    }

    afficherGrille();
    if (etatJeu == 1) {
        printf("Fin du jeu ! Le %s gagne !\n", tour == 1 ? "joueur" : "IA");
    } else {
        printf("Match nul !\n");
    }

    return 0;
}
