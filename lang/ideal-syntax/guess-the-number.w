import printstr from wlib
import printnb from wlib
import input from wlib
import rand from wlib

extern
fun main(Void): I32
{
    var attempts: I32 = 0
    var mystery_number: I32 = rand(1, 10)

    printstr("Guess the number (between 1 and 100): \n")

    while (input_number != mystery_number) {
        cst input_number: I32 = input()
        attempts = attempts + 1

        if (input_number < mystery_number) {
            printstr("It's more!\n")
        } 
        elif (input_number > mystery_number) {
            printstr("It's less!\n")
        } 
        else {
            printnb("Congratulations, you've found the mystery number in ", attempts, " attempts!\n")
        }
    }    

    return 0
}
