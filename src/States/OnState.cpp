
#include "OnState.h"
#include <Arduino.h>
#include <stack>
#include "LightController.cpp"

void OnState::off(LightController& controller) const {
    controller.offCommand.execute();
    //controller.setState(&controller.offState);
}
void OnState::on(LightController& controller) const {
   

}
void OnState::bright(LightController& controller) const {

    std::stack<LightState*>& states = controller.getLastState();
    LightState* topState = states.top();
    if(topState == &controller.brightLightState){
        return;
    }
    else if(topState == &controller.normalLightState){
        controller.oneUpCommand.execute();
    }
    else if(topState == &controller.dimLightState){
        controller.oneUpCommand.execute();
        controller.oneUpCommand.execute();
    }
}
void OnState::normal(LightController& controller) const {

    std::stack<LightState*>& states = controller.getLastState();
    LightState* topState = states.top();
    Serial.println(topState->toString());
    if(topState == &controller.normalLightState){
        return;
    }
    else if(topState == &controller.brightLightState){
        controller.oneDownCommand.execute();

    }
    else if(topState == &controller.dimLightState){
        controller.oneDownCommand.execute();
    }

}
void OnState::dim(LightController& controller) const {

    std::stack<LightState*>& states = controller.getLastState();
    LightState* topState = states.top();
    if(topState == &controller.dimLightState){
        return;
    }
    else if(topState == &controller.normalLightState){
        controller.oneDownCommand.execute();
    }
    else if(topState == &controller.brightLightState){
        controller.oneDownCommand.execute();
        controller.oneDownCommand.execute();
    }
    
}
String OnState::toString() const {
    return "OnLightState";
}

