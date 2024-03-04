#include <stdio.h>

int main(void) {
    int nombreMystere = 10;
    int saisie = 0;
    
    printf("Devinez le nombre (entre 1 et 100) : ");
    
    while (saisie != nombreMystere) {
        scanf("%d", &saisie);
        
        if (saisie < nombreMystere) {
            printf("C'est plus !\n");
        } 
        else if (saisie > nombreMystere) {
            printf("C'est moins !\n");
        } 
        else {
            printf("Bravo, vous avez trouvé le nombre mystère !\n");
        }
    } 
    
    return 0;
}
