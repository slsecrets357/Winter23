/*

Write a function that keeps on asking the user to input a number as long as the provided number is not divisible by 2 and 3.

For every number that the user inputs, the function will calculate the square of the number and prints it to the screen.

*/

#include <iostream>

void askUser();

int main()
{
    askUser();
    return 0;
}

void askUser()
{
    /*
    keeps on asking the user to input a number as long as the provided number is not divisible by 2 and 3.

For every number that the user inputs, the function will calculate the square of the number and prints it to the screen.
    */
    int num;
    while(true){
        std::cout << "Enter a number: ";
        std::cin >> num;
        if(num % 2 == 0 && num % 3 == 0){
            std::cout << "The square of the number is: " << num * num << std::endl;
            return;
        }
        std::cout << "The square of the number is: " << num * num << std::endl;
    }
}

int getLength(char* myString) {
    int length = 0;
    char c = *myString;
    while (c != '\0') {
        length++;
        c++;
    }
}

