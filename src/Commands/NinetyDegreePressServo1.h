#include <iostream>
#include <ESP32Servo.h>
#include "Command.h"


class NinetyDegreePressServo1 : public Command {
private:
    Servo& servo;

public:
    NinetyDegreePressServo1(Servo& servo ) : servo(servo) {}

    void execute() const override {
        servo.write(90);

    }
};