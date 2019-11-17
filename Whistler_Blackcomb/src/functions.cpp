#include "../include/functions.h"

unsigned int Functions::Factorial( unsigned int number ) {
    return number <= 1 ? number : Factorial(number-1)*number;
}