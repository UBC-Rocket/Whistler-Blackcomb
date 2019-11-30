/*
State Machine Header
Function: Main state machine of the Whistler-Blackcomb rocket
Implements state switching and calls state dependent functions
*/

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

/*Includes-----------------------------------------------------------*/


/*Variables----------------------------------------------------------*/
enum FlightState{
    STANDBY,
    FUELING,
    IGNITION,
    ASCENT,
    FIRST_DESCENT,
    SECOND_DESCENT,
    FINAL_DESCENT,
    LANDED
};

/*Functions----------------------------------------------------------*/
void switchState(FlightState * currentState, FlightState newState);
void stateMachine(FlightState * currentState);
int test(int one, int two);

#endif