 //main.cc
#define CATCH_CONFIG_MAIN
#include "../lib/catch.hpp"
#include "../include/functions.h"
#include "../include/statemachine.h"

TEST_CASE( "Factorials are computed", "[factorial]" ) {
    //CHECK( Functions::Factorial(0) == 1 );
    CHECK( Functions::Factorial(1) == 1 );
    CHECK( Functions::Factorial(2) == 2 );
    CHECK( Functions::Factorial(3) == 6 );
    CHECK( Functions::Factorial(10) == 3628800 );
}

TEST_CASE("Testing state machine") {
    FlightState flightState;
    
    //Check initialized state
    CHECK( flightState.getState() == Fueling);

    //Move to next state
    flightState.setState(Ignition);

    //Check set state to another operation state
    CHECK ( flightState.getState() == Ignition);

    //Set error state
    flightState.setState(WinterContingency);

    //Check successful entry of error state
    CHECK( flightState.getState() == WinterContingency);

    //Try to move out of error state to normal operation state - should not work
    flightState.setState(Recovery1);

    //Ensure that it has not left error state
    CHECK( flightState.getState() == WinterContingency);


}

TEST_CASE("Testing") {
    SUCCEED();
}