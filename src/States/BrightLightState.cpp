#include "BrightLightState.h"
#include "LightController.cpp"
#include <Arduino.h>

void BrightLightState::off(LightController& controller) const {
    controller.offCommand.execute();

}
void BrightLightState::on(LightController& controller) const {

}
void BrightLightState::bright(LightController& controller) const {

}
void BrightLightState::normal(LightController& controller) const {
    controller.oneDownCommand.execute();

}
void BrightLightState::dim(LightController& c) const {
    c.ninetyDegreePressServo2.execute();
    c.thirtyDegreePressServo1.execute();
    c.readLDR();
    





   
}
String BrightLightState::toString() const{
    return "BrightLightState";
}



