#include "DimLightState.h"
#include "LightController.cpp"
#include <Arduino.h>

void DimLightState::off(LightController& controller) const {
    controller.offCommand.execute();
    //controller.setState(&controller.offState);
}

void DimLightState::on(LightController& controller) const {
}

void DimLightState::bright(LightController& controller) const {
    controller.oneUpCommand.execute();

}

void DimLightState::normal(LightController& controller) const {
    controller.oneUpCommand.execute();
    delay(200);
    controller.oneUpCommand.execute();

}

void DimLightState::dim(LightController& controller) const {
}
String DimLightState::toString() const{
    return "DimLightState";
}

