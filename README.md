see first approximative BNF [first-approximative-BNF](./docs/first-approximative-BNF.txt)

TODO : [TODO.txt](TODO.txt)


## Syntax 

see full syntax here : [SYNTAX.md](./docs/SYNTAX.md)

- try with the test.sh files

```python
# comments are not supported yet
# else statements are not supported yet

# declare constants
let i = 0;  

# declare variable
var a = 2; 

# if 
if (a == i) {
  let r = 0;
}
```


## How to use the compiler
```shell
--asm   # compile for asm only
--cpp   # compile for cpp only
# if you put nothing or both il will compile for both

-o
--output-dir "mydirectory"  # put output files in ./mydirectory ()
# if nothing is provided, it will put output files in the build
# the directory is automaticaly created

-v
--verbose # display in the terminal all the outputs

# Exemples
compiler test.sh
compiler test.sh --asm -v --output-dir "build"
compiler test.sh --asm --cpp --output-dir "mydirectory" 
``` 

<br>

## Developpement

### Build 
```shell
$ make
``` 

### Codind style

```shell
$ clang-format -i ./src/*
``` 

check this file : [coding-style.txt](./docs/coding-style.txt)