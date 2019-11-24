//#ifdef STATEMACHINE_H
//#define STATEMACHINE_H

enum FlightStates{
    Fueling = 0,
    Ignition = 1,
    Ascent = 2,
    Recovery1 = 3,
    Recovery2 = 4,
    Recovery3 = 5,
    Recovery4 = 6,
    FinalDescent = 7,
    Landed = 8,
    WinterContingency = 9
};

class FlightState{
    public:
        FlightState();
        void setState(FlightStates new_state);
        FlightStates getState();
    private:
        FlightStates currentState;
};


//#endif