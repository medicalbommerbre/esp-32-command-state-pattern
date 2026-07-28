#pragma once
#include <Arduino.h>
#include <string.h>
class LightController;
class LightState{

    public:
        virtual void off(LightController& controller) const = 0;
        virtual void on(LightController& controller) const = 0;
        virtual void bright(LightController& controller) const = 0;
        virtual void normal(LightController& controller) const = 0;
        virtual void dim(LightController& controller) const = 0;
 
        virtual String toString() const =0;
        virtual ~LightState() = default;
};
