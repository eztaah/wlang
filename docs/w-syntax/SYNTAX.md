Types
```
unsigned int : u32 u64
signed int   : i32 i64
float        : f32 f64 
character    : c8
```

Declarations and Definitions:
```c
cst a: i32 = 2
var c: i32 = 2
var b: c8 = 'c'
```

Pointers:
```c
mut x: i32 = 5
cst x_ptr: &i32 = &x
*x_ptr = 10
```

Array:
```c
cst array1: &i32 = {2, 4, 9, 3}
cst array2: &i32 = {0}

cst element = *(array + 1)
```

Blocks
```c
// if statement
if (a == b) {
} 

// while loop
while (1) {
}

// for loop
for (i in 0..10) {
}
for (i in 0..=10) {
}
```

Functions:
```c
fun foo1(a: i32, b: f64): i32 {
    output a + b
}
fun foo2(cst a: &i32, var b: &f64): void {
    output
}
foo1(var1, var2)
foo2(&var1, &var2)
```


Preprocessing: 
```c
%replace TRUE -> 0

%import print %from 
```



FILE INCLUSION
