/*
State Machine Source

Function: State Machine of the Whistler Blackcomb Rocket
Implements state switching and caling of state specific functions
*/

/*Includes------------------------------------------------------*/
#include "includes\statemachine.h"

/*Constants-----------------------------------------------------*/

/*Functions-----------------------------------------------------*/
void switchState(FlightState * currentState, FlightState newState){
    *currentState = newState;
}

void stateMachine(FlightState * currentState){
    switch(*currentState){

    }
}

int test(int one, int two){
    return one +two;
}