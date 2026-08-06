#include <iostream>
#include <ESP32Servo.h>
#include "Command.h"


class NinetyDegreePressServo2 : public Command {
private:
    Servo& servo;

public:
    NinetyDegreePressServo2(Servo& servo ) : servo(servo) {}

    void execute() const override {
        servo.write(90);

    }
};