 //main.cc
#define CATCH_CONFIG_MAIN
#include "../lib/catch.hpp"
#include "../includes/statemachine.h"

TEST_CASE("Testing state machine") {
    FlightState flightState;
    
    //Check initialized state
    CHECK( flightState.getState() == Fueling);

    //Move to next state
    flightState.setState(Ignition);

    //Check set state to another operation state
    CHECK(flightState.getState() == Ignition);

    //Set error state
    flightState.setState(WinterContingency);

    //Check successful entry of error state
    CHECK(flightState.getState() == WinterContingency);

    //Try to move out of error state to normal operation state - should not work
    flightState.setState(Recovery1);

    //Ensure that it has not left error state
    CHECK(flightState.getState() == WinterContingency);
}

// TEST_CASE("Testing voter module class") {

// }

TEST_CASE("Testing") {
    SUCCEED();
}