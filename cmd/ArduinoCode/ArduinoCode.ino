#include <Keypad.h>
#include <Servo.h>
#include <Wire.h>
#include <Adafruit_VL53L0X.h> // Library for VL53L0X

// Create VL53L0X object
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// Pin definitions
const int servoPin = 3;       // Servo motor pin
const int buzzerPin = 5;      // Buzzer pin
const int pirSensorPin = 4;   // PIR motion sensor pin
const int redLedPin = 2;      // Red LED pin for alarm
const int greenLedPin = 13;   // Green LED pin for access granted

// Garbage basket system
Servo garbageServo;
const int garbageOpenAngle = 90; // Servo angle to open the basket
const int garbageCloseAngle = 0; // Servo angle to close the basket
const int garbageThreshold = 20; // Distance threshold in cm to open the basket

// Security system
const String correctPassword = "1234";
String enteredPassword = "";
bool securityArmed = false;

// Keypad setup
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
    {'1', '2', '3'},
    {'4', '5', '6'},
    {'7', '8', '9'},
    {'*', '0', '#'}};
byte rowPins[ROWS] = {6, 7, 8, 9};
byte colPins[COLS] = {10, 11, 12};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup()
{
  Serial.begin(9600);
  Wire.begin();

  // Initialize VL53L0X sensor
  if (!lox.begin())
  {
    Serial.println("Failed to boot VL53L0X sensor!");
    while (1);
  }
  Serial.println("VL53L0X sensor initialized.");

  // Initialize servo and LEDs
  garbageServo.attach(servoPin);
  garbageServo.write(garbageCloseAngle);

  pinMode(pirSensorPin, INPUT);
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  // Turn off all outputs initially
  digitalWrite(redLedPin, LOW);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(buzzerPin, LOW);
}

void loop()
{
  char key = keypad.getKey();
  if (key)
    handleKeypadInput(key);

  handleGarbageBasketSystem();
  handleSecuritySystem();

  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n');
    processESP32Command(command);
  }
}

// Function to handle garbage basket system
void handleGarbageBasketSystem()
{
  VL53L0X_RangingMeasurementData_t measure;

  // Read distance from VL53L0X sensor
  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4) // If valid reading
  {
    long distance = measure.RangeMilliMeter / 10; // Convert mm to cm
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Open or close the basket based on distance
    if (distance > 0 && distance < garbageThreshold)
    {
      garbageServo.write(garbageOpenAngle);
      Serial.println("Basket Opened.");
    }
    else
    {
      garbageServo.write(garbageCloseAngle);
      Serial.println("Basket Closed.");
    }
  }
  else
  {
    Serial.println("Out of range");
  }
}

// Function to handle security system
void handleSecuritySystem()
{
  if (securityArmed)
  {
    int motionDetected = digitalRead(pirSensorPin);
    digitalWrite(redLedPin, motionDetected);
    digitalWrite(buzzerPin, motionDetected);
    sendToESP32("SECURITY:" + String(motionDetected));
  }
}

void handleKeypadInput(char key)
{
  Serial.print("Key pressed: ");
  Serial.println(key);

  if (key == '#')
  {
    if (enteredPassword == correctPassword)
    {
      Serial.println("Access granted.");
      digitalWrite(greenLedPin, HIGH);
      delay(1000);
      digitalWrite(greenLedPin, LOW);
      securityArmed = false;
    }
    else
    {
      Serial.println("Access denied.");
      digitalWrite(redLedPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      delay(1000);
      digitalWrite(redLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
    }
    enteredPassword = "";
  }
  else if (key == '*')
  {
    enteredPassword = "";
  }
  else
  {
    enteredPassword += key;
    Serial.println("Password so far: " + enteredPassword);
  }
}

void processESP32Command(String command)
{
  if (command.startsWith("SECURITY_ARMED"))
  {
    int armedState = command.substring(15).toInt();
    securityArmed = (armedState == 1);
    Serial.println(securityArmed ? "Security armed." : "Security disarmed.");
  }
  else
  {
    Serial.println("Unknown command received: " + command);
  }
}

void sendToESP32(const String &data)
{
  Serial.println(data);
}
