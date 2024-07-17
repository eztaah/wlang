Declarations and Definitions:
```c
int a = 2;
char b = 'c';
float c = 3.14;
double d = 9.81;

// Declaration of a structure
struct Point {
    int x;
    int y;
};

// Declaration of a union
union Data {
    int i;
    float f;
    char str[20];
};

// Declaration of an enumeration
enum boolean { NO, YES };
```

<br>

Operators:
```c
a + b; // Addition
a - b; // Subtraction
a * b; // Multiplication
a / b; // Division
a % b; // Modulo (remainder of integer division)

a == b; // Equal to
a != b; // Not equal to
a > b;  // Greater than
a < b;  // Less than
a >= b; // Greater than or equal to
a <= b; // Less than or equal to

a && b; // Logical AND operator
a || b; // Logical OR operator
!a;     // Logical NOT operator
```

<br>

Functions:
```c
// Function declaration
int myFunction(int a, int b) {
    return a + b;
}

// Function call
int result = myFunction(3, 4);
```

<br>

Pointers:
```c
int var = 5;
int *ptr = &var; // Pointer declaration

*ptr = 10; // Modifying the value pointed to
```

<br>

Array:
```c
int array[10]; // Declaration of an array of 10 integers

array[0] = 1; // Assigning the value 1 to the first element
```