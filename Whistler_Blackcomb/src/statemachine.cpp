#include "../includes/statemachine.h"

/* Constructor for FlightState class definition */
FlightState::FlightState(){
    currentState = Fueling;
}

/* void switchState(FlightStates){}
 * @brief  Switches state machine state.
 * @param  FlightState new_state - State machine state to switch to.
 * @return void.
 */
void FlightState::setState(FlightStates newState){
    if(currentState == newState){
        return; //do nothing, we are already in desired state
    }
    if(currentState != WinterContingency) //don't want to switch out of WINTER_CONTINGENCY accidentally
        currentState = newState;
}

/* FlightStates getState(void){}
 * @brief Returns the current state
 * @return FlightStates enumeration state
 */
FlightStates FlightState::getState(){
    return currentState;
}

/* void loop()
 * @brief Loops through the state machine logic to control program flow
 */
void FlightState::loop(){
    switch(currentState){
        case(Fueling):
            break;
        case(Ignition):
            break;
    }
}