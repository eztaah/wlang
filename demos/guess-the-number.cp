var saisie = 0;
var nombreDeCoups = 0;
var nombreMystere = 10;

print("Devinez le nombre (entre 1 et 100) : \n");

while (saisie != nombreMystere) {
    saisie = input();
    nombreDeCoups = nombreDeCoups + 1;

    if (saisie < nombreMystere) {
        print("C'est plus !\n");
    } 
    if (saisie > nombreMystere) {
        print("C'est moins !\n");
    } 
    if (saisie == nombreMystere) {
        print("Bravo, vous avez trouvé le nombre mystère en ", nombreDeCoups, " coups !\n");
    }
}
