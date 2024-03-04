# install this vscode extension to have color


cst nombreMystere: i64 = 10;
var saisie: i64 = 0;

print("Devinez le nombre (entre 1 et 100) : \n");

while (saise != nombreMystere) {
    saisie = input();

    if (saisie < nombreMystere) {
        print("C'est plus !\n");
    } 
    elif (saisie > nombreMystere) {
        print("C'est moins !\n");
    } 
    else {
        print("Bravo, vous avez trouvé le nombre mystère !\n");
    }
}
