#include <stdio.h>
#include <stdlib.h>

int main() {
    int saisie = 0;
    int nombreDeCoups = 0;
    
    int nombreMystere = 10;
    
    printf("Devinez le nombre (entre 1 et 100) : ");
    
    while (saisie != nombreMystere) {
        scanf("%d", &saisie);
        nombreDeCoups++;
        
        if (saisie < nombreMystere) {
            printf("C'est plus !\n");
        } 
        else if (saisie > nombreMystere) {
            printf("C'est moins !\n");
        } 
        else {
            printf("Bravo, vous avez trouvé le nombre mystère en %d coups !\n", nombreDeCoups);
        }
    } 
    
    return 0;
}
