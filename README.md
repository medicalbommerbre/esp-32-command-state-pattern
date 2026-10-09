# ESP32 Light Controller

A personal project where I use an ESP32 to control lighting with physical buttons, servo motors, sensors, and Wi-Fi.

I built this project to experiment with embedded programming and get more experience with C++. I also wanted to try using the **State and Command design patterns** in something practical instead of just learning about them in theory.

The idea was to build a lighting system that I could control manually using buttons, but also remotely through Wi-Fi.

## Features

- **Physical controls:** Buttons for switching between dim, normal, and bright lighting modes, plus a touch input for turning the light on and off.
- **Servo motors:** Two servos that physically operate the light switch.
- **Motion detection:** Automatically toggles the light when motion is detected.
- **Light detection:** An LDR sensor measures ambient brightness and helps detect changes in lighting.
- **Wi-Fi control:** A simple HTTP server for controlling the light remotely.
- **OTA updates:** Firmware can be updated over Wi-Fi.
- **State tracking:** Keeps track of the current lighting mode.

## Hardware

The project uses:

- ESP32 development board
- 2 servo motors
- 3 push buttons
- Touch button
- Motion sensor
- LDR (Light Dependent Resistor)
- Jumper wires and power supply

### Pin configuration

| Component | GPIO |
|---|---|
| Servo 1 | 16 |
| Servo 2 | 17 |
| Dim button | 27 |
| Normal button | 25 |
| Bright button | 33 |
| Motion sensor | 32 |
| Touch button | 26 |
| LDR sensor | 34 |

## How it works

The ESP32 controls two servo motors that interact with the physical light switch.

I added buttons to control the different brightness levels without needing a phone or computer. There is also a touch button for turning the light on and off.

The motion sensor can toggle the light automatically, while the LDR detects changes in brightness. This helps keep track of the lighting state, even when the light is adjusted physically.

I also added an HTTP server so I can control the light using requests over my local Wi-Fi network.

## Design Patterns

One of the main reasons I started this project was to experiment with design patterns in embedded C++.

### State Pattern

I used the **State Pattern** to manage the different lighting modes.

The project has several states:

- `OffState`
- `OnState`
- `DimLightState`
- `NormalLightState`
- `BrightLightState`

Each state defines how the controller should respond to different actions.

This keeps the lighting logic separated and makes it easier to add or change behavior without ending up with a lot of nested `if` statements.

### Command Pattern

I used the **Command Pattern** to handle the actions performed by the servo motors.

Instead of directly controlling the servos from every part of the program, the actions are separated into commands, such as turning the light on, turning it off, or adjusting the brightness.

This makes it easier to reuse commands and change how the hardware behaves without changing the rest of the controller.

## Software

The project is written in **C++**, using the Arduino framework and PlatformIO.

Libraries and tools used:

| Tool / Library | Purpose |
|---|---|
| PlatformIO | Building and uploading firmware |
| Arduino Framework | ESP32 programming |
| ESP32Servo | Controlling servo motors |
| WiFiManager | Setting up Wi-Fi |
| WebServer | Handling HTTP requests |
| ArduinoOTA | Updating firmware wirelessly |

## Getting Started

### Requirements

- ESP32 development board
- USB cable
- [Visual Studio Code](https://code.visualstudio.com/)
- [PlatformIO](https://platformio.org/)

### Installation

1. Clone the repository:

   ```bash
   git clone https://github.com/medicalbommerbre/esp-32-command-state-pattern.git
   ```

2. Open the project in VS Code with PlatformIO installed.

3. Connect your ESP32 to your computer.

4. For the first upload, configure `platformio.ini` to use USB uploading:

   ```ini
   upload_protocol = esptool
   ```

5. Build and upload the project using PlatformIO.

   ```bash
   pio run
   pio run --target upload
   ```

### Wi-Fi Setup

The project uses WiFiManager, so Wi-Fi credentials don't have to be hardcoded.

If the ESP32 doesn't have a saved Wi-Fi connection, it starts a configuration access point called `ESP32-Setup`.

Connect to it and follow the configuration portal to enter your Wi-Fi credentials.

Once connected, the ESP32 prints its IP address to the serial monitor.

## HTTP API

I added a basic HTTP server so the lighting can also be controlled over Wi-Fi.

The server runs on port `80`.

| Endpoint | Action |
|---|---|
| `GET /on` | Turn the light on |
| `GET /off` | Turn the light off |
| `GET /dim` | Set dim brightness |
| `GET /normal` | Set normal brightness |
| `GET /bright` | Set bright brightness |
| `GET /state` | Get the current lighting state |
| `GET /LDR` | Get the current LDR reading |

For example, to turn the light on:

```bash
curl http://<ESP32_IP>/on
```

Or to change the brightness:

```bash
curl http://<ESP32_IP>/bright
```

The API is intended for use on a trusted local network since authentication hasn't been implemented.


There are still a few things I'd like to experiment with:

- A simple web interface for controlling the light
- Better sensor calibration
- More reliable detection of manual lighting changes
- Saving the current state after restarting the ESP32
- Improving error handling
- Adding tests for state transitions

## About

This is a personal hobby project, mainly built to learn more about embedded systems, C++, and software design patterns.



