
#include "OffState.h"
#include "LightController.cpp"
#include <Arduino.h>

void OffState::off(LightController& controller) const {

}
void OffState::on(LightController& controller) const {
    controller.onCommand.execute();
    //controller.setState(&controller.onState);
}
void OffState::bright(LightController& controller) const {

}
void OffState::normal(LightController& controller) const {

}
void OffState::dim(LightController& controller) const {
    
}
String OffState::toString() const{
    return "OffState";
}




