#include "BrightLightState.h"
#include "LightController.cpp"
#include <Arduino.h>

void BrightLightState::off(LightController& controller) const {
    controller.offCommand.execute();
    //controller.setState(&controller.offState);
}
void BrightLightState::on(LightController& controller) const {

}
void BrightLightState::bright(LightController& controller) const {

}
void BrightLightState::normal(LightController& controller) const {
    controller.oneDownCommand.execute();

}
void BrightLightState::dim(LightController& controller) const {
    controller.oneDownCommand.execute();
    delay(200);
    controller.oneDownCommand.execute();
}
String BrightLightState::toString() const{
    return "BrightLightState";
}



