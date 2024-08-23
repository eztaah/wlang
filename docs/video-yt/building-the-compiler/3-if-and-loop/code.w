: ajout des operations aritmethiques (==, !=, <=, &&, etc)
: ajout du if, else
: ajour du loop, break


: if statements 
<64> var1 = 0;
if (var1 == 0) {
    var1 = 10;
}
else {
    var1 = -1;
}

: loops
<64> i = 0;
loop {
    i = i + 1;

    if (i == 10) {
        break;
    }
}

ret i + var1;