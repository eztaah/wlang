var input_number: i64 = 0
var attempts: i64 = 0
var mystery_number: i64 = 10

print("Guess the number (between 1 and 100): \n")

while (input_number != mystery_number) {
    input_number = input()
    attempts = attempts + 1

    if (input_number < mystery_number) {
        print("It's more!\n")
    } 
    if (input_number > mystery_number) {
        print("It's less!\n")
    } 
    if (input_number == mystery_number) {
        print("Congratulations, you've found the mystery number in ", attempts, " attempts!\n")
    }
}