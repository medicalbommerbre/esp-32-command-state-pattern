#include <iostream>
#include <ESP32Servo.h>
#include "Command.h"


class ThirtyDegreePressServo1 : public Command {
private:
    Servo& servo;

public:
    ThirtyDegreePressServo1(Servo& servo ) : servo(servo) {}

    void execute() const override {
        servo.write(30);
    }
};