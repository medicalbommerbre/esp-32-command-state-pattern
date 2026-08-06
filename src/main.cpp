#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>
#include <ArduinoOTA.h>
#include "States/LightController.cpp"


constexpr int SERVO_1_PIN = 16;
constexpr int SERVO_2_PIN = 17;

constexpr int DIM_BUTTON_PIN = 27;
constexpr int NORMAL_BUTTON_PIN = 25;
constexpr int BRIGHT_BUTTON_PIN = 33;

constexpr int MOTION_SENSOR_PIN = 32;
constexpr int LDR_PIN = 34;
constexpr int TOUCH_PIN = 26;

bool sensorDisabled = false;
unsigned long lastMotionCommandTime = 0;
unsigned long lastMotionDetected = 0;
unsigned long lastLDRReadTime = 0;
constexpr unsigned long LDR_INTERVAL_MS = 250;
constexpr int LDR_CHANGE_THRESHOLD = 250;
constexpr unsigned long MOTION_COOLDOWN_MS = 10000;

bool previousTouchState = false;
bool previousDimButtonState = false;
bool previousNormalButtonState = false;
bool previousBrightButtonState = false;
bool previousMotionState = false;
//This bool determines what way the buttons needs to be pressed based on a sudden change in brightness without any commands being executed recently
bool backwardMode = false;

int latestBrightness = 0;
int lastLDRRead = 0;

Servo servo1;
Servo servo2;
LightController light(servo1, servo2);

WebServer server(80);

/**
 * Returns true only when a button changes from not pressed to pressed.
 */
bool wasJustPressed(bool currentState, bool previousState)
{
  return currentState && !previousState;
}

/**
 * Handles physical button and sensor input.
 */
void handleInputs(
  bool touchPressed,
  bool dimPressed,
  bool normalPressed,
  bool brightPressed,
  bool motionDetected
)
{

  if (touchPressed)
  {
    String currentState = light.getState()->toString();

    if (currentState == light.offState.toString())
    {
      light.on();
      lastMotionCommandTime = millis();
    }
    else
    {
      light.off();
      lastMotionCommandTime = millis();
    }
    return;
  }
  if (normalPressed)
  {
    light.normal();
    lastMotionCommandTime = millis();
    return;
  }
  if (brightPressed)
  {
    light.bright();
    lastMotionCommandTime = millis();
    return;
  }
  if (dimPressed)
  {
    light.dim();
    lastMotionCommandTime = millis();
    return;
  }
  if(motionDetected){
    String currentState = light.getState()->toString();
    if(millis()-lastMotionDetected >=30000){// if last motion was more then 30 seconds ago the sensor will work, otherwise it will be locked(otherwise it will start spamming the servos. if needed change to 10 seconds(10000)
      if (currentState == light.offState.toString())
      {
        light.on();
        lastMotionCommandTime = millis();
      }
      else
      {
        light.off();
        lastMotionCommandTime = millis();
      }
    lastMotionDetected = millis();
    }
    return;
  }
  }

/**
 * Web server route handlers.
 */
void handleOn()
{
  light.on();
  server.send(200, "text/plain", "OK");
}

void handleOff()
{
  light.off();
  server.send(200, "text/plain", "OK");
}
void handleLDR(){
  latestBrightness = analogRead(LDR_PIN);
  server.send(
    200,
    "text/plain",
    "OK: " + String(latestBrightness)
  );
}

void handleBright()
{
  light.bright();
  server.send(200, "text/plain", "OK");
}

void handleNormal()
{
  light.normal();
  server.send(200, "text/plain", "OK");
}

void handleDim()
{
  light.dim();
  server.send(200, "text/plain", "OK");
}

void handleGetState()
{
  server.send(
    200,
    "text/plain",
    light.getState()->toString()
  );
}

void readLDR() {

  latestBrightness = analogRead(LDR_PIN);

  int brightnessChange = latestBrightness - lastLDRRead;
  LightState* currentState = light.getState();
  Serial.println(latestBrightness);
  bool commandWasOverOneSecondAgo =
      millis() - lastMotionCommandTime >= 1000UL;

  if (brightnessChange >= 200 &&
      currentState != &light.onState) {

      // Light went ON
      if (!light.getLastState().empty()) {
          LightState* tempState = light.getLastState().top();
          light.setState(tempState);
      }

      if (commandWasOverOneSecondAgo) {
          light.setBackwards(true);
      }else{
        light.setBackwards(false);
      }

  } else if (brightnessChange <= -100 &&
              currentState != &light.offState) {

      // Light went OFF
        if (commandWasOverOneSecondAgo) {
          light.setBackwards(true);
        }else{
          light.setBackwards(false);
        }


      light.setState(&light.offState);

  } else if (brightnessChange >= 100) {
      // Room became brighter
      if (currentState == &light.dimLightState) {
          light.setState(&light.normalLightState);
      } else if (currentState == &light.normalLightState) {
          light.setState(&light.brightLightState);
      }

  } else if (brightnessChange <= -100) {
      // Room became darker
      if (currentState == &light.brightLightState) {
          light.setState(&light.normalLightState);
      } else if (currentState == &light.normalLightState) {
          light.setState(&light.dimLightState);
      }
  }

    lastLDRRead = latestBrightness;

}
void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

void configureWebServer()
{
  server.on("/on", HTTP_GET, handleOn);
  server.on("/off", HTTP_GET, handleOff);
  server.on("/bright", HTTP_GET, handleBright);
  server.on("/normal", HTTP_GET, handleNormal);
  server.on("/dim", HTTP_GET, handleDim);
  server.on("/state", HTTP_GET, handleGetState);
  server.on("/LDR", HTTP_GET, handleLDR);

  server.onNotFound(handleNotFound);
  server.begin();
}

void configurePins()
{
  pinMode(MOTION_SENSOR_PIN, INPUT);
  pinMode(TOUCH_PIN, INPUT_PULLDOWN);
  pinMode(DIM_BUTTON_PIN, INPUT_PULLUP);
  pinMode(NORMAL_BUTTON_PIN, INPUT_PULLUP);
  pinMode(BRIGHT_BUTTON_PIN, INPUT_PULLUP);
}

void connectToWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.setAutoReconnect(true);
  WiFi.setSleep(false);

  WiFiManager wifiManager;
  wifiManager.setConfigPortalTimeout(180);

  Serial.println("Connecting to Wi-Fi...");

  bool connected = wifiManager.autoConnect("ESP32-Setup");

  if (!connected)
  {
    Serial.println("Wi-Fi connection failed");
    Serial.println("Restarting ESP32...");

    delay(3000);
    ESP.restart();
  }

  Serial.println("Wi-Fi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}
void configureOTA(){  
  ArduinoOTA
    .onStart([]() {
      Serial.println("OTA Start");
    })
    .onEnd([]() {
      Serial.println("\nOTA End");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress * 100) / total);
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]\n", error);
    });

  ArduinoOTA.begin();
  ArduinoOTA.setHostname("ESP32-LIGHTS");
  Serial.println("Ready for OTA");

}

void setup()
{
  Serial.begin(115200);
  delay(500);

  Serial.println();
  servo1.attach(SERVO_1_PIN);
  servo2.attach(SERVO_2_PIN);

  configurePins();
  connectToWiFi();
  configureWebServer();
  configureOTA();
}
void loop()
{
  ArduinoOTA.handle();
  server.handleClient();
  if (millis() - lastLDRReadTime >= LDR_INTERVAL_MS)
  {
    lastLDRReadTime = millis();
    readLDR();
  }
  bool touchActive = digitalRead(TOUCH_PIN) == HIGH;
  bool dimButtonActive = digitalRead(DIM_BUTTON_PIN) == LOW;
  bool normalButtonActive = digitalRead(NORMAL_BUTTON_PIN) == LOW;
  bool brightButtonActive = digitalRead(BRIGHT_BUTTON_PIN) == LOW;

  bool motionActive = digitalRead(MOTION_SENSOR_PIN) == HIGH;

  bool motionDetected = wasJustPressed(
    motionActive,
    previousMotionState
  );

  bool touchPressed = wasJustPressed(
    touchActive,
    previousTouchState
  );

  bool dimPressed = wasJustPressed(
    dimButtonActive,
    previousDimButtonState
  );

  bool normalPressed = wasJustPressed(
    normalButtonActive,
    previousNormalButtonState
  );

  bool brightPressed = wasJustPressed(
    brightButtonActive,
    previousBrightButtonState
  );

  handleInputs(
    touchPressed,
    dimPressed,
    normalPressed,
    brightPressed,
    motionDetected
    );

  previousTouchState = touchActive;
  previousDimButtonState = dimButtonActive;
  previousNormalButtonState = normalButtonActive;
  previousBrightButtonState = brightButtonActive;
  previousMotionState = motionActive;
  delay(10);
}