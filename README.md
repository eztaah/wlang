# wlang

L'objectif ici est de créer un language de programmation haut niveau, le plus simple possible
Le compilateur est codé en C et transform your high level code to x86_64 GNU assembly.

Vision : 
    - Le compilateur ne fera aucune optimisation, il fait ce que qu'on lui demande et c'est tout.
    - 


A Visual Studio Code extension is available for syntax highlighting :    
[https://marketplace.visualstudio.com/items?itemName=eztaah.w](https://marketplace.visualstudio.com/items?itemName=eztaah.w)

<br>

## Language syntax 

Voici à quoi ressemble la syntaxe du language : 

Ici nous avons un programme qui additionne 2 nombres et qui affiche la somme sur le terminal : 
<img src="./docs/language-preview.png" alt="image" width="475">

Quels sont les points clés : 
- le mot clé return est obligatoire meme si la fonction ne retourne rien
- les "fonction" précedé par un @ sont directement convertis en assembleur lors de la compilation, il n'y a donc pas de definition de ces fonctions dans la librarie standard. 
- Le seul type existant est le I64 (entier signé sur 64 bits), il faut garder en tete que l'on peut y stocker un code ascii et meme une adresse mémoire. Il est donc possible de manipuler des caracteres et meme des adresse memoire.
- Il n'est pas possible d'ecrire des instructions en dehors des fonctions
- Ne pas oublier d'appeler la fonction exit à la fin de la "fonction" _start

<br>

## How to use the compiler

Pour compiler votre code, vous devez utiliser le programme wlangc (voir ci dessous pour build le compilateur)

Voici comment l'utiliser : 

```
$ wlangc --help                                    
Usage:
    wlangc <file.w> [options]     compile a source file with optional flags.
    wlangc --version              display compiler version and exit.
    wlangc --help                 display this help message and exit.

Options:
    -v, --verbose                   output additional information
    -d, --dev-mode                  activate dev mode (add comments in the asm code, ...)
    -e, --to-executable             assemble and link the asmed assembly code into an executable
                                        - GNU assembler (as) and GNU linker (ld) will be needed during compilation time.
                                        - the generated executable will only run on x86_64 architecture and requires a Linux system.

``` 

<br>

## Building the compiler from source

Navigate to the projet directory and execute this command :
```shell
cd compiler
make release
``` 

A binary named `wlangc` will be generated in your current directory.