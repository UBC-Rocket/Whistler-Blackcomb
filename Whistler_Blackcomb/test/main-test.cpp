 //main.cc
#define CATCH_CONFIG_MAIN
#include "../lib/catch.hpp"
#include "../include/functions.h"

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    //CHECK( Functions::Factorial(0) == 1 );
    CHECK( Functions::Factorial(1) == 1 );
    CHECK( Functions::Factorial(2) == 2 );
    CHECK( Functions::Factorial(3) == 6 );
    CHECK( Functions::Factorial(10) == 3628800 );
}

TEST_CASE("Testing") {
    SUCCEED();
}