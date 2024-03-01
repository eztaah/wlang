Types
```
unsigned   : u8 u16 u32 u64 u128
signed int : i8 i16 i32 i64 i128
float      : f32 f64 
character  : char
```

Declarations and Definitions:
```c
cst a: i32 = 2
var c: i32 = 2
var b: char = 'c'
cst myString: string = "Test"
myString.length 

// Array
cst myArray: i32[] = [1, 2, 3] 
myArray[1] = 1
myArray.push(4)

// Structure
struct Point {
    cst x: i32
    cst x: i32
}
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
for (cst elt in array) {
}
```

Functions:
```c
fun foo1(a: i32, b: f64): i32 {
    return
}
fun foo2(cst a: &i32, var b: &f64): chr {
    return
}
foo1(var1, var2)
foo2(&var1, &var2)
```