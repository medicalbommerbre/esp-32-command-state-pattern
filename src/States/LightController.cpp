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

#include "Commands/OneDownCommandBackward.h"
#include "Commands/OneUpCommandBackward.h"
#include "Commands/OffCommandBackward.h"
#include "Commands/OnCommandBackward.h"

#include "Commands/NinetyDegreePressServo1.h"
#include "Commands/ThirtyDegreePressServo1.h"
#include "Commands/NinetyDegreePressServo2.h"
#include "Commands/ThirtyDegreePressServo2.h"

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

    OffCommandBackward offCommandBackward;
    OnCommandBackward onCommandBackward;
    OneDownCommandBackward oneDownCommandBackward;
    OneUpCommandBackward oneUpCommandBackward;

    NinetyDegreePressServo1 ninetyDegreePressServo1;
    ThirtyDegreePressServo1 thirtyDegreePressServo1;
    NinetyDegreePressServo2 ninetyDegreePressServo2;
    ThirtyDegreePressServo2 thirtyDegreePressServo2;

    bool backwards = false;
    LightController(Servo& s1, Servo& s2)
        : lightState(&dimLightState),
          offCommand(s1),
          onCommand(s2),
          oneDownCommand(s2, s1),
          oneUpCommand(s2, s1),
          offCommandBackward(s2),
          onCommandBackward(s1),
          oneDownCommandBackward(s1, s2),
          oneUpCommandBackward(s1,s2),

          ninetyDegreePressServo1(s1),
          ninetyDegreePressServo2(s2),
          thirtyDegreePressServo1(s1),
          thirtyDegreePressServo2(s2)
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

    void setBackwards(bool i){
        backwards = i;
    }

    void readLDR() {

        latestBrightness = analogRead(LDR_PIN);

        int brightnessChange = latestBrightness - lastLDRRead;
        LightState* currentState = light.getState();
        Serial.println(latestBrightness);
        bool commandWasOverOneSecondAgo =
            millis() - lastMotionCommandTime >= 1000UL;

        if (brightnessChange >= 200 &&
            currentState != &light.onState) {

            // Light went ON
            if (!light.getLastState().empty()) {
                LightState* tempState = light.getLastState().top();
                light.setState(tempState);
            }

            if (commandWasOverOneSecondAgo) {
                light.setBackwards(true);
            }else{
                light.setBackwards(false);
            }

        } else if (brightnessChange <= -100 &&
                    currentState != &light.offState) {
        
            // Light went OFF
                if (commandWasOverOneSecondAgo) {
                light.setBackwards(true);
                }else{
                light.setBackwards(false);
                }


            light.setState(&light.offState);

        } else if (brightnessChange >= 100) {
            // Room became brighter
            if (currentState == &light.dimLightState) {
                light.setState(&light.normalLightState);
            } else if (currentState == &light.normalLightState) {
                light.setState(&light.brightLightState);
            }

        } else if (brightnessChange <= -100) {
            // Room became darker
            if (currentState == &light.brightLightState) {
                light.setState(&light.normalLightState);
            } else if (currentState == &light.normalLightState) {
                light.setState(&light.dimLightState);
            }
        }

    lastLDRRead = latestBrightness;

}

    void off()    { lightState->off(*this); }
    void on()     { lightState->on(*this); }
    void bright() { lightState->bright(*this); }
    void normal() { lightState->normal(*this); }
    void dim()    { lightState->dim(*this); }
};