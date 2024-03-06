# this is a comment

# declare variables
var a = 0;  

# declare constants
cst b = 9; 

# print function
print(a);
print("b is equal to : ", b, "\n");

# input function
a = input();        # for exemple we enter 5

# if statement
if (a == 5) {
    print("wow, a is equal to 5\n");
}
else {
    print("...");
}

# while loop
while (a < b) {
    a = a + 1;
}


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
