#include <iostream>
#include <ESP32Servo.h>
#include "Command.h"

class OneDownCommand : public Command {
private:
    Servo& servo;
    Servo& servo2;

public:
    OneDownCommand(Servo& servo, Servo& servo2) : servo(servo), servo2(servo2) {}

    void execute() const override {
        servo2.write(90);
        delay(200);
        servo2.write(30);
        delay(200);
        servo.write(90);
        delay(200);
        servo.write(30);
    }

};