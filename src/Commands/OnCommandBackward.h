#include <iostream>
#include <ESP32Servo.h>
#include "Command.h"

class OnCommandBackward : public Command {
private:
    Servo& servo;
  
public:
    OnCommandBackward(Servo& servo) : servo(servo) {}

    void execute() const override {
        servo.write(90);
        delay(200);
        servo.write(30);
    }
};