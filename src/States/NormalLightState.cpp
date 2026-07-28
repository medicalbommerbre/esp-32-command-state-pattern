
#include "NormalLightState.h"
#include "LightController.cpp"
#include <Arduino.h>

void NormalLightState::off(LightController& controller) const {
    controller.offCommand.execute();
    //controller.setState(&controller.offState);
}   
void NormalLightState::on(LightController& controller) const {
   
}
void NormalLightState::bright(LightController& controller) const {
    controller.oneUpCommand.execute();
    delay(200);
    controller.oneUpCommand.execute();
    //controller.setState(&controller.brightLightState);
}
void NormalLightState::normal(LightController& controller) const {

}
void NormalLightState::dim(LightController& controller) const {
    controller.oneDownCommand.execute();
    //controller.setState(&controller.dimLightState);
}
String NormalLightState::toString() const{
    return "NormalLightState";
}



