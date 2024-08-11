#def PI 3.14
#inc "lib.wh"

print_digit(digit);

add(a, b)
{
    : do a sycall for something:
    @syc(2, 0, 0, 0, 0, 0);

    ret a + b;
}

main()
{
    :define two cst integers:
    
    array[8]

    array: [8]x100;    :array est l'adresse de la premiere boite


    [8];        créer une boite :on peut peut pas y acceder car on a pas sauvegardé son adresse:
    [8] <
    7x[16]      : créer 7 boites de 16 bits: :on ne peux toujours pas y acceder:



    array[8]x7;        : array est un label, il contiendra l'adresse relative de la boite lors de la compilation ou de la premiere boite, il contient uniquement l'adresse, il ne connait pas le contenu:

    array <- 4;

    *(array + 4) <- 5;



    int hello = 2;
    int* ptr = &hello;

    hello[3] = ;

    ptr = hello;



    num10[8];

    num10 est uniuqment un label qui permet à mon compilateur de savoir à quelle adresse relative se trouve ma boite (-4(%rbp)), num10 n'est donc pas une boite.

    box_of_box[6*8];    

    [&box_of_box + 8] <- ()9;

    

    num0[8] <- 9;

    num1[8];           :num1 est une boite de 8 octets: :une boite a une adresse et un contenu:
    [&num1] <- 8;

    ptr[64];
    [&ptr] <- &num1;        je met dans la boite qui est stocké à l'adresse &ptr l'adresse de num1
    [#ptr] <- 2;   :je modifie le contenu de la boite sotcké à l'adresse : contenu de la boite ptr, cette boite contient une adresse
    


    :calculate the sum of num1 and num2, then print the result:
    sum[64] <- add(#num1, #num2);
    print_digit(#sum);

    
    :test if statement: 
    if (#num1 < 2) {
        print_digit(#num1);
    }
    else {
        print_digit(#num1 + 2);
    }

    :loop:
    loop {
        [&num1] = #num1 + 2;
        
        if (#num1 == #num2) {
            break;
        }

        else {
            continue;
        }
    }


    :cast:

    fun convert_int_to_char(int) {
        return int + 42;
    }

    small_box[8] <- b101010111;
    big_box[64] <- 3;
    big_box[64] <- -3;
    big_box[64] <- ;

    new_box[64] <- #small_box;
    [&big_box] <- #big_box + #new_box;



    ret 0;
}

CONCLUSION

Convertir un signed en unsigned ne change en aucun cas la valeur, mais la facon dont on interprete ce nombre binaire. 

On a aussi des types qui donne la taille de la boite, int, long, short. Le type char, donne la taille de la boite ainsi que la facon dont interpreter l'entier (qui peut etre non singé ou singé mais le keyword char ne donne aucune information sur quel type on choisit). Le type *int, donne une indication sur la taille de la boite, ainsi que de la facon dont on doit interpreter l'entier dans cette boite. 

Pour recap on a :
    - qualificateur (signed et unsigned) : informe si il faut interpreter le nombre binaire comme un entier signé (ca2) ou un entier non signé, il output un entier.

    - type : il informe la taille de la boite ainsi que comment il faut interpreter l'entier.
                - char dit que la boite contient 8 bits et que il faudra interpreter l'entier (apres passage dans signed ou unsigned) comme un code ascii

    - le mot clé 'float' est un cas particulier, il est a la fois un qualificateur et un type, car il explique comment interpreter le nombre binaire directement, il n'y a pas de passage en entier.


interssant :
il y a 2 layers
    - comment on interprete le nombre binaire (le qualificateur nous donne l'information) -> entier
    - comment on interprete l'entier (le type nous donne l'information)



CONCLUSION MON LANGUAGE : 
    - il n'y a pas de qualificateur, le nombre binaire doit etre interprete comme un entier signé, 

    - la taille et la facon dont il faut interpreter cet entier signé sont separé en 2 partie
        - num1[8] : indique la taille de la boite
        - la facon dont il faut interpreter cet entier est à gerer par le developpeur, MAIS LE FAIT DE SAVOIR SI IL FAUT INTERPRETER L'ENTIER COMME SIGNÉ OU NON SIGNÉ EST DEFINI PAR LE LANGUAGE  (uniquemnt signé)

    - il n'y a pas de type float, car ajouter le type float dans mon language me forcerais à ajouter un type à chaque boite, car le compilateur aurait besoin de savoir si il doit utiliser le add ou addss lorsqu'il additionne le contenu de la boite. 



: problemes

- les commentaire empeche d'utiliser les : 

- !sysc et !fun_call doivent faire la meme chose, car pr l'instant il y en a un qui utilise call et l'autre qui utilise syscall
: