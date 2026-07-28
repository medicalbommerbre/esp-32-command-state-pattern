#include <Arduino.h>
#include <ESP32Servo.h>
#include <WiFi.h>
#include <WebServer.h>
#include <WiFiManager.h>

#include "States/LightController.cpp"


constexpr int SERVO_1_PIN = 16;
constexpr int SERVO_2_PIN = 17;

constexpr int DIM_BUTTON_PIN = 27;
constexpr int NORMAL_BUTTON_PIN = 25;
constexpr int BRIGHT_BUTTON_PIN = 33;

constexpr int MOTION_SENSOR_PIN = 32;
constexpr int TOUCH_PIN = 26;

bool sensorDisabled = false;
unsigned long lastMotionCommandTime = 0;
constexpr unsigned long MOTION_COOLDOWN_MS = 10000;

bool previousTouchState = false;
bool previousDimButtonState = false;
bool previousNormalButtonState = false;
bool previousBrightButtonState = false;

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
  
  if (touchPressed || motionDetected)
  {
    String currentState = light.getState()->toString();
    if (currentState == light.offState.toString())
    {
      light.on();
      light.setState(light.lastState());
    }
    else
    {
      light.off();
      light.setState(&light.offState);
    }
    return;
  }

  if (normalPressed)
  {
    light.normal();
    return;
  }

  if (brightPressed)
  {
    light.bright();
    return;
  }

  if (dimPressed)
  {
    light.dim();
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

void handlePostValue() {
  if (!server.hasArg("value")) {
    server.send(400, "text/plain", "Missing value");
    return;
  }
  latestBrightness = server.arg("value").toInt();

  int brightnessChange = latestBrightness - lastLDRRead;
  String state = light.getState()->toString();

  // Serial.print("Previous: ");
  // Serial.print(lastLDRRead);
  // Serial.print(" | Current: ");
  // Serial.print(latestBrightness);
  // Serial.print(" | Change: ");
  // Serial.print(brightnessChange);
  // Serial.print(" | State: ");
  // Serial.println(light.getState()->toString());

  if (brightnessChange >= 200) {
    // Room became brighter
    if (state == light.dimLightState.toString()) {
      light.setState(&light.normalLightState);
    }
    else if (state == light.normalLightState.toString()) {
      light.setState(&light.brightLightState);
    }
  }
  else if (brightnessChange <= -200) {
    // Room became darker
    if (state == light.brightLightState.toString()) {
      light.setState(&light.normalLightState);
    }
    else if (state == light.normalLightState.toString()) {
      light.setState(&light.dimLightState);
    }
  }
  lastLDRRead = latestBrightness;

  server.send(200, "text/plain", "OK");
}

void handleGetValue() {
  server.send(200, "text/plain", String(latestBrightness));
}

void handleNotFound() {
  server.send(404, "text/plain", "Not found");
}

/**
 * Registers all HTTP endpoints.
 */
void configureWebServer()
{
  server.on("/on", HTTP_GET, handleOn);
  server.on("/off", HTTP_GET, handleOff);
  server.on("/bright", HTTP_GET, handleBright);
  server.on("/normal", HTTP_GET, handleNormal);
  server.on("/dim", HTTP_GET, handleDim);
  server.on("/state", HTTP_GET, handleGetState);
  server.on("/value", HTTP_POST, handlePostValue);
  server.on("/value", HTTP_GET, handleGetValue);

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
}

void loop()
{
  server.handleClient();

  bool touchActive = digitalRead(TOUCH_PIN) == HIGH;
  bool dimButtonActive = digitalRead(DIM_BUTTON_PIN) == LOW;
  bool normalButtonActive = digitalRead(NORMAL_BUTTON_PIN) == LOW;
  bool brightButtonActive = digitalRead(BRIGHT_BUTTON_PIN) == LOW;

  bool motionDetected = digitalRead(MOTION_SENSOR_PIN) == HIGH;

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

  delay(10);
}