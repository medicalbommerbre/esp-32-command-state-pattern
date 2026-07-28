#pragma once
#include "LightState.h"
#include <Arduino.h>
#include <string.h>
class LightController;

class OffState : public LightState {
public:
    void off(LightController& controller) const override;
    void on(LightController& controller) const override;
    void bright(LightController& controller) const override;
    void normal(LightController& controller) const override;
    void dim(LightController& controller) const override;

    String toString() const;
};