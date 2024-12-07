#include <Keypad.h>
#include <Servo.h>

// Pin definitions for Garbage Basket System
const int trigPin = 2;           // Ultrasonic sensor trigger pin
const int echoPin = 4;           // Ultrasonic sensor echo pin
Servo garbageServo;              // Servo motor object for controlling garbage basket
const int garbageOpenAngle = 90; // Servo angle to open the basket
const int garbageCloseAngle = 0; // Servo angle to close the basket
const int garbageThreshold = 20; // Distance threshold in cm to open the basket

// Pin definitions for Security System
const int pirSensorPin = 8; // PIR motion sensor pin
const int redLedPin = 12;    // Red LED pin for alarm (Separate from keypad pins)
const int greenLedPin = 13; // Green LED pin for access granted (Separate from keypad pins)
const int buzzerPin = 3;   // Buzzer pin for alarm

// Keypad setup for user authentication (4x3 Keypad)
const byte ROWS = 4; // Four rows
const byte COLS = 3; // Three columns
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
// Connect keypad rows and columns to these pins
byte rowPins[ROWS] = {5, 6, 7, 8};
byte colPins[COLS] = {9, 10, 11};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Variables for keypad input and security system
String enteredPassword = "";       // Buffer for storing entered password
const String correctPassword = "1234"; // Correct password for access
bool securityArmed = false;         // Security system armed state

void setup() {
  Serial.begin(9600); // Serial communication with ESP32

  // Configure garbage basket system pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  garbageServo.attach(3); // Attach servo to pin 3
  garbageServo.write(garbageCloseAngle); // Ensure basket is closed at startup

  // Configure security system pins
  pinMode(pirSensorPin, INPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Turn off all outputs initially
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(buzzerPin, LOW);
}

void loop() {
  // Check for keypad input and process it
  char key = keypad.getKey();
  if (key) handleKeypadInput(key);

  // Handle the garbage basket system
  handleGarbageBasketSystem();

  // Handle the security system
  handleSecuritySystem();

  // Check for incoming commands from the ESP32
  if (Serial.available()) {
    String command = Serial.readStringUntil('\n'); // Read command from ESP32
    processESP32Command(command); // Process the command
  }
}

// Function to handle keypad input
void handleKeypadInput(char key) {
  if (key == '#') {
    // Validate entered password
    if (enteredPassword == correctPassword) {
      Serial.println("Access granted.");
      digitalWrite(greenLedPin, HIGH); // Indicate access granted
      delay(1000);
      digitalWrite(greenLedPin, LOW);
      securityArmed = false; // Disarm security system
    } else {
      Serial.println("Access denied.");
      digitalWrite(redLedPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      delay(1000);
      digitalWrite(redLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
    }
    enteredPassword = ""; // Clear the password buffer
  } else if (key == '*') {
    // Clear the entered password
    enteredPassword = "";
  } else {
    // Append the pressed key to the entered password
    enteredPassword += key;
  }
}

// Function to handle the garbage basket system
void handleGarbageBasketSystem() {
  long duration, distance;

  // Trigger ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure distance
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

// Function to handle the security system
void handleSecuritySystem() {
  if (securityArmed) {
    int motionDetected = digitalRead(pirSensorPin); // Check for motion
    if (motionDetected == HIGH) {
      digitalWrite(redLedPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      sendToESP32("SECURITY:1"); // Notify ESP32 of alarm
    } else {
      digitalWrite(redLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
      sendToESP32("SECURITY:0"); // Notify ESP32 of alarm
    }
  } else {
    // Security system disarmed
    digitalWrite(redLedPin, LOW);
    digitalWrite(buzzerPin, LOW);
    sendToESP32("SECURITY:0"); // Notify ESP32 of alarm
  }
}

// Function to send data to ESP32
void sendToESP32(const String &data) {
  Serial.println(data);
}

// Function to process commands from ESP32
void processESP32Command(String command) {
  if (command.startsWith("SECURITY_ARMED")) {
    int armedState = command.substring(15).toInt();
    securityArmed = (armedState == 1);
    Serial.println(securityArmed ? "Security armed." : "Security disarmed.");
  }
}
