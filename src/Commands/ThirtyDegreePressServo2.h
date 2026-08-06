#include <iostream>
#include <ESP32Servo.h>
#include "Command.h"


class ThirtyDegreePressServo2 : public Command {
private:
    Servo& servo;

public:
    ThirtyDegreePressServo2(Servo& servo ) : servo(servo) {}

    void execute() const override {
        servo.write(30);
    }
};