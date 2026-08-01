### Vision
- Video technique, je n'ai pas peur de rentrer dans les details, il faut que le viewer ai appris enormement de chose random dans cette video 


### Titre et Miniature
Titre : 
- I Made My Own Programming Language In C
- How I Made My Own Programming Language
    - Mettre le mot "Compiled" car tt le monde fait un interpreteur
    - mettre que je le fait en C (compiled, et C sont vraiment les mots qui vont me faire sortir du lot)

- How to Create a Compiler 

Miniature  :
- Logo W à gauche
- MY OWN LANGUAGE (text)
- assembly code en background

<br>

### Idees randoms

- expliquer comment coder en assembleur
- montrer mon graphe githu
- il faut montrer les tests en python

------------------------------------

<br>
<br>


## 1. Qu’est ce que je vais montrer dans cette video

1. Faire un recap de tout ce que je vais montrer dans cette video
    - I will show you how i build the compiler in C
    - how i was able to create a standard library (print, input) from scratch with my own programming langauge
    - and how i created a morpion, pong game et meme recréer la fonction ls 


- Expliquer toutes les grandes etapes de la video 

- What i was able to build at the end was really crazy... (rester justqu'a la fin)



But, first of all, how to create a programming language? 

## X. Interpreted or compiled 

1. Il y a 2 types, compilés et interpreté 
    - Expliquer la difference

2. Parler un peu plus en profondeur de compiler, en parlant de l'assembleur etc (we will talk about that later)

X. wlang est un lanaguge compilé 


## X. Choosing a name and a syntax

X. Explication du nom du langauge

1. Syntax du language

X. Explication de la vision derreire ce langugage
    - pas de types (uniquement des boites)
        - expliquer le fait que en C char dit juste qu'il faut interpreter l'entier comme ceci


## X. Part1 Building a small compiler (arithmetic only) (pas de var assigment, pas de fonctions)

Le but de cet partie est de créer un compilateur fonctionnel le plus petit possible, pour bien expliquer les differentes parties, l'implemntation en C et l'assembleur. Il faut vraiment bien expliquer pour que le viewer se dise, ok c'est simple
Points clés qui vont etre abordés dans cette partie : 
    - comment créer un lexer, visualiser l'outpout
    - comment créer un parser, visualiser l'output
    - recap assembly 
    - creation d'un asm generator


## X. variables


## if, else, loops 


## X. functions
    - add functions
    - add & dereferencement

Le  but ici est de vraiment passer du temps sur la notion de fichier objet, le linkage, comment on peut compiler plusieurs fichier, appeler des fonctions externes
Points clés abordés dans cette partie :
    - il faut respecter le system V abi
    - Compilation mulitfichier
    - On peut appeler des fonctions de la libc
    - On va faire bcp d'asembleur pour comprendre comment on call une fonction

A la fin de cette partie, le viewer doit comprendre que tt les languages sont compilés en assembleur, et que on peut communiquer entre differents languages du moememnt qu'il respecte la meme convention (ici system v abi)

- at this point, we can create a hello world (with malloc, free, and puts), however it is not very clean


## AAA creating our first game
On utilise juste la fonction putchar de la libc, input de la libc, 
On essaye utilise que des fonctions de la libc


## strings, preprocessor
    - preprocesseur 
    - tableaux statics (parler de ma gestion des chaines de caracteres)
    - add >>, <<, &, |, #  (binary arithmetic operators)
    - ajout dereferencement
    - ajout de tests avec un script python (pour verifier si tout marche bien)



## X. Building the standard library
L'etape precedente à permis de clean le language car maintenant que nous allons faire une lib standard, il doit etre solide et supporter des grosses codebase.

- Expliquer que printf, etc sont juste des wrapper autout des systemes call

- Expliquer que je vais recréer une librairie from scratch, dans mon propre langauge

- Le but ici est de ne pas utiliser la libc, mais de tout faire a partir de syscall


## X. Playing with wlang

*Creation d'un jeu de morpion* 
Ici on va pouvoir utiliser notre librairie standard, et notre type string

- Faire plusieurs fichiers pour bien montrer la compilation multifile



*Creating a pong with raylib*

*Creating the ls function*



## X. Outro

- le source code est dispo sur github 
