#include <ESP32Servo.h>
#include "LightState.h"
#include "OffState.h"
#include "OnState.h"
#include "BrightLightState.h"
#include "NormalLightState.h"
#include "DimLightState.h"
#include <stack>
#include <iostream>
#include "Commands/OffCommand.h"
#include "Commands/OnCommand.h"
#include "Commands/OneDownCommand.h"
#include "Commands/OneUpCommand.h"

class LightController {
public:
    std::stack<LightState*> states;
    OffState offState;
    OnState onState;
    BrightLightState brightLightState;
    NormalLightState normalLightState;
    DimLightState dimLightState;
    LightState* lightState;

    OffCommand offCommand;
    OnCommand onCommand;
    OneDownCommand oneDownCommand;
    OneUpCommand oneUpCommand;

    LightController(Servo& s1, Servo& s2)
        : lightState(&dimLightState),
          offCommand(s1),
          onCommand(s2),
          oneDownCommand(s2, s1),
          oneUpCommand(s2, s1)  
    {
        states.push(&dimLightState);
    }

    void setState(LightState* state) {
        lightState = state;
        std::stack<LightState*> copy = states;

        if(state != &offState && state != &onState){
            states.push(state);
        }
        if(states.size() >10){
            states.pop();
            states.pop();
        }
    }
    std::stack<LightState*>& getLastState(){
        return states;
    }

    LightState* getState() {
        return lightState;
    }
    LightState* lastState(){
        LightState* temp = states.top();
        return temp;
    }

    void off()    { lightState->off(*this); }
    void on()     { lightState->on(*this); }
    void bright() { lightState->bright(*this); }
    void normal() { lightState->normal(*this); }
    void dim()    { lightState->dim(*this); }
};