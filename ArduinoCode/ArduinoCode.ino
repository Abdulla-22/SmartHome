#include <Keypad.h>
#include <Servo.h>

// Pin definitions for Lighting System
const int outdoorLedPin = 3;   // Outdoor LED pin
const int indoorLedPin = 5;    // Indoor LED pin
const int ldrPin = A0;         // LDR sensor pin
const int soundSensorPin = A1; // Analog pin connected to sound sensor

// Pin definitions for Garden System
const int soilMoisturePin = A2; // Soil moisture sensor pin
const int rainSensorPin = 7;    // Rain sensor digital pin (updated to digital)
const int waterPumpPin = 6;     // Water pump control pin

// Pin definitions for Security System
const int pirSensorPin = 8; // PIR motion sensor pin
const int redLedPin = 9;    // Red LED pin for alarm
const int greenLedPin = 10; // Green LED pin for access granted
const int buzzerPin = 11;   // Buzzer pin for alarm

// Pin definitions for Smart Environment Control
const int tempSensorPin = A3;   // Temperature sensor pin
const int acGreenLedPin = 12;   // Green LED for air conditioner ON
const int acRedLedPin = 13;     // Red LED for air conditioner OFF

// Pin definitions for Garbage Basket System
const int trigPin = 2;          // Ultrasonic sensor trigger pin
const int echoPin = 4;          // Ultrasonic sensor echo pin
Servo garbageServo;             // Servo motor object
const int garbageOpenAngle = 90; // Servo angle to open the basket
const int garbageCloseAngle = 0; // Servo angle to close the basket
const int garbageThreshold = 20; // Distance threshold in cm to open the basket

// Constants
const unsigned long clapDelay = 500;   // Minimum delay between claps (in milliseconds)
const int ldrThreshold = 500;          // Threshold for LDR sensor
const int moistureThreshold = 999;     // Threshold for soil moisture sensor (adjust based on your sensor)
const String correctPassword = "1234"; // Set your desired password
const int tempThreshold = 30;          // Temperature threshold for air conditioner

// Keypad setup
const byte ROWS = 4; // Four rows
const byte COLS = 4; // Four columns
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};
byte rowPins[ROWS] = {2, 3, 4, 5}; // Connect keypad ROW0, ROW1, ROW2, ROW3 to these pins
byte colPins[COLS] = {6, 7, 8, 9}; // Connect keypad COL0, COL1, COL2, COL3 to these pins
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables for states
unsigned long lastClapTime = 0; // Time of the last detected clap
bool outdoorLightAuto = true;   // Automatic control for outdoor lighting
bool indoorLightAuto = true;    // Automatic control for indoor lighting
bool indoorLightState = false;  // Indoor light state (on/off)
bool outdoorLightState = false; // Outdoor light state (on/off)
bool gardenAutoMode = true;     // Automatic mode for the garden system
bool securityArmed = true;      // Security system armed mode
bool environmentAutoMode = true; // Automatic mode for environment system

// Buffer for keypad input
String enteredPassword = "";

void setup()
{
  Serial.begin(9600); // Start Serial communication with ESP32

  // Lighting system pin configurations
  pinMode(outdoorLedPin, OUTPUT);
  pinMode(indoorLedPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(soundSensorPin, INPUT);

  // Garden system pin configurations
  pinMode(soilMoisturePin, INPUT);
  pinMode(rainSensorPin, INPUT);
  pinMode(waterPumpPin, OUTPUT);

  // Security system pin configurations
  pinMode(pirSensorPin, INPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Smart environment control pin configurations
  pinMode(tempSensorPin, INPUT);
  pinMode(acGreenLedPin, OUTPUT);
  pinMode(acRedLedPin, OUTPUT);

  // Garbage basket system pin configurations
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  garbageServo.attach(3); // Attach servo to pin 3
  garbageServo.write(garbageCloseAngle); // Ensure basket is closed at startup

  // Turn off all actuators at startup
  digitalWrite(outdoorLedPin, LOW);
  digitalWrite(indoorLedPin, LOW);
  digitalWrite(waterPumpPin, LOW);
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(buzzerPin, LOW);
  digitalWrite(acGreenLedPin, LOW);
  digitalWrite(acRedLedPin, LOW);

  Serial.println("Arduino is ready.");
}

void loop()
{
  // Check for keypad input
  char key = keypad.getKey();
  if (key)
  {
    handleKeypadInput(key);
  }

  // Check if data is available from ESP32
  if (Serial.available() > 0)
  {
    String command = Serial.readStringUntil('\n'); // Read the command
    command.trim();                                // Remove extra whitespace
    processESP32Data(command);                     // Handle the command
  }

  // Handle lighting system
  handleOutdoorLighting();
  handleIndoorLighting();

  // Handle garden system
  handleGardenSystem();

  // Handle security system
  handleSecuritySystem();

  // Handle Environment Control
  handleEnvironmentControl();
  handleGarbageBasketSystem();
}

// Handle keypad input
void handleKeypadInput(char key)
{
  if (key == '#')
  {
    // Check entered password
    if (enteredPassword == correctPassword)
    {
      Serial.println("Access granted.");
      digitalWrite(greenLedPin, HIGH); // Turn on green LED
      delay(1000);
      digitalWrite(greenLedPin, LOW);
      securityArmed = false; // Disarm the security system
    }
    else
    {
      Serial.println("Access denied.");
      digitalWrite(redLedPin, HIGH); // Turn on red LED
      digitalWrite(buzzerPin, HIGH); // Sound the alarm
      delay(1000);
      digitalWrite(redLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
    }
    enteredPassword = ""; // Clear the buffer
  }
  else if (key == '*')
  {
    // Clear the entered password
    enteredPassword = "";
    Serial.println("Password cleared.");
  }
  else
  {
    // Add the key to the entered password
    enteredPassword += key;
    Serial.println("Key pressed: " + String(key));
  }
}

// Function to parse and handle incoming ESP32 data
void processESP32Data(String command)
{
  int delimiterIndex = command.indexOf(':');
  if (delimiterIndex != -1)
  {
    String tag = command.substring(0, delimiterIndex);
    String value = command.substring(delimiterIndex + 1);
    int intValue = value.toInt(); // Convert value to integer

    if (tag == "IN_DOOR_LIGHT_STATE")
    {
      handleIndoorState(intValue);
    }
    else if (tag == "IN_DOOR_LIGHT_AUTO")
    {
      handleIndoorAutoState(intValue);
    }
    else if (tag == "OUT_DOOR_LIGHT_AUTO")
    {
      handleOutdoorAutoState(intValue);
    }
    else if (tag == "OUT_DOOR_LIGHT_STATE")
    {
      handleOutdoorState(intValue);
    }
    else if (tag == "SECURITY_ARMED")
    {
      securityArmed = (intValue == 1);
      Serial.println(securityArmed ? "Security system armed." : "Security system disarmed.");
    }
    else
    {
      Serial.println("Unknown tag: " + tag);
    }
  }
  else
  {
    Serial.println("Invalid command format: " + command);
  }
}

// Handle indoor light state
void handleIndoorState(int state)
{
  indoorLightState = (state == 1);
  Serial.println("Indoor Light State: " + String(indoorLightState));
}

// Handle indoor light auto/manual mode
void handleIndoorAutoState(int state)
{
  indoorLightAuto = (state == 1);
  Serial.println(indoorLightAuto ? "Indoor light set to AUTO mode." : "Indoor light set to MANUAL mode.");
}

// Handle outdoor light auto/manual mode
void handleOutdoorAutoState(int state)
{
  outdoorLightAuto = (state == 1);
  Serial.println(outdoorLightAuto ? "Outdoor light set to AUTO mode." : "Outdoor light set to MANUAL mode.");
}

// Handle outdoor light state
void handleOutdoorState(int state)
{
  outdoorLightState = (state == 1);
  Serial.println("Outdoor Light State: " + String(outdoorLightState));
}

// Handle security system
void handleSecuritySystem()
{
  if (securityArmed)
  {
    int motionDetected = digitalRead(pirSensorPin);
    if (motionDetected == HIGH)
    {
      digitalWrite(redLedPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      sendDataToESP("SECURITY:ALARM");
    }
    else
    {
      digitalWrite(redLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
    }
  }
  else
  {
    digitalWrite(redLedPin, LOW);
    digitalWrite(buzzerPin, LOW);
  }
}

// Control outdoor lighting
void handleOutdoorLighting()
{
  if (outdoorLightAuto)
  {
    int ldrValue = analogRead(ldrPin); // Read LDR value
    digitalWrite(outdoorLedPin, (ldrValue < ldrThreshold) ? HIGH : LOW);
  }
  else
  {
    digitalWrite(outdoorLedPin, outdoorLightState ? HIGH : LOW);
  }
}

// Control indoor lighting with sound sensor
void handleIndoorLighting()
{
  if (indoorLightAuto)
  {
    int soundDetected = analogRead(soundSensorPin); // Read the sound sensor
    if (soundDetected > 30)
    {
      unsigned long currentTime = millis();
      if (currentTime - lastClapTime > clapDelay)
      {                                       // Ensure delay between claps
        indoorLightState = !indoorLightState; // Toggle the LED state
        lastClapTime = currentTime;
      }
    }
  }
  digitalWrite(indoorLedPin, indoorLightState ? HIGH : LOW);
}

// Handle garden system
void handleGardenSystem()
{
  int soilMoistureValue = analogRead(soilMoisturePin); // Read soil moisture value
  int rainSensorState = digitalRead(rainSensorPin);    // Read rain sensor state (digital mode)

  if (gardenAutoMode)
  {
    if (rainSensorState == LOW)
    {                                  // Rain detected
      digitalWrite(waterPumpPin, LOW); // Turn off water pump
    }
    else if (soilMoistureValue > moistureThreshold)
    {
      digitalWrite(waterPumpPin, HIGH); // Turn on water pump
    }
    else
    {
      digitalWrite(waterPumpPin, LOW); // Turn off water pump
    }
  }
  sendDataToESP("SOIL_MOISTURE:" + String(soilMoistureValue));
  sendDataToESP("RAIN_SENSOR:" + String(rainSensorState));
}

// Handle garbage basket system
void handleGarbageBasketSystem() {
  // Measure distance using ultrasonic sensor
  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;

  // Open or close basket based on distance
  if (distance > 0 && distance < garbageThreshold) {
    garbageServo.write(garbageOpenAngle);
    Serial.println("Basket Opened.");
  } else {
    garbageServo.write(garbageCloseAngle);
    Serial.println("Basket Closed.");
  }
}

// Handle environment control
void handleEnvironmentControl() {
  if (environmentAutoMode) {
    int temperature = analogRead(tempSensorPin) / 10; // Simulate temperature in °C

    // Control air conditioner with LEDs
    if (temperature > tempThreshold) {
      digitalWrite(acGreenLedPin, HIGH);
      digitalWrite(acRedLedPin, LOW);
      Serial.println("Air Conditioner ON.");
    } else {
      digitalWrite(acGreenLedPin, LOW);
      digitalWrite(acRedLedPin, HIGH);
      Serial.println("Air Conditioner OFF.");
    }

    // Send data to ESP32 for monitoring
    sendDataToESP("TEMPERATURE:" + String(temperature));
  }
}

// Send data to ESP32
void sendDataToESP(const String &data)
{
  Serial.println(data);
}
