var saisie = 0;
var nombreMystere = 10;

print("Devinez le nombre (entre 1 et 100) : \n");

while (saisie != nombreMystere) {
    saisie = input();

    if (saisie < nombreMystere) {
        print("C'est plus !\n");
    } 
    if (saisie > nombreMystere) {
        print("C'est moins !\n");
    } 
    if (saisie == nombreMystere) {
        print("Bravo, vous avez trouvé le nombre mystère !\n");
    }
}




# var a = 3;

# print("A est égal à ", a, "\n");

# print("Entrez un nombre : ");
# var b = input();

# print("Le nombre que vous avez entré est : ", b);

# ceci
# while (a < b) {
#     a = a + 1;
#     print("valeur de a : ");
#     print(a);
#     print("\n");
# }


# if (a > b) {
#     println("true");
# }
# else {
#     println("false");
# }
