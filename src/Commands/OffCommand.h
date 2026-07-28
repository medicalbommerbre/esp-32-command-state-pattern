#include <iostream>
#include <ESP32Servo.h>
#include "Command.h"


class OffCommand : public Command {
private:
    Servo& servo;

public:
    OffCommand(Servo& servo ) : servo(servo) {}

    void execute() const override {
        servo.write(90);
        delay(200);
        servo.write(30);
    }
};