/*
State Machine Source

Function: State Machine of the Whistler Blackcomb Rocket
Implements state switching and caling of state specific functions
*/

/*Includes------------------------------------------------------*/
#include "statemachine.h"

/*Constants-----------------------------------------------------*/

/*Functions-----------------------------------------------------*/
void switchState(FlightState * currentState, FlightState newState){
    *currentState = newState;
}

void stateMachine(FlightState * currentState){
    switch(*currentState){

    }
}