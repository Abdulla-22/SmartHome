#include <Keypad.h>

// Pin definitions
const int buzzerPin = 5;       // Buzzer pin
const int pirSensorPin = 4;    // PIR motion sensor pin
const int vibrationSensorPin = 3; // Vibration sensor pin
const int redLedPin = 2;       // Red LED pin for alarm
const int greenLedPin = 13;    // Green LED pin for access granted

// Security system
const String correctPassword = "1234";
String enteredPassword = "";
bool securityArmed = false;
bool prevSecurityArmed = false;
int counter = 0;
int LastAccess = 0;

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

  pinMode(pirSensorPin, INPUT);
  pinMode(vibrationSensorPin, INPUT); // Set vibration sensor pin as input
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

  handleSecuritySystem();

  if (Serial.available())
  {
    String command = Serial.readStringUntil('\n');
    processESP32Command(command);
  }
}

// Function to handle security system
void handleSecuritySystem()
{
  if (counter > 3) 
  {
    securityArmed = true;
  }
  if (securityArmed)
  {
    int motionDetected = digitalRead(pirSensorPin);
    int vibrationDetected = digitalRead(vibrationSensorPin);

    // Trigger alarm if motion or vibration is detected
    if (motionDetected || vibrationDetected)
    {
      digitalWrite(redLedPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      sendToESP32("SECURITY:1");
      delay(500); // Alarm delay
    }
    else
    {
      sendToESP32("SECURITY:0");
      // digitalWrite(redLedPin, LOW);
      // digitalWrite(buzzerPin, LOW);
    }
  }

  // Check for changes in securityArmed and send only if changed
  if (securityArmed != prevSecurityArmed)
  {
    sendToESP32(securityArmed ? "SECURITY_ARMED:1" : "SECURITY_ARMED:0");
    prevSecurityArmed = securityArmed; // Update the previous state
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
      digitalWrite(redLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
      Serial.println("Access granted.");
      digitalWrite(greenLedPin, HIGH);
      delay(1000);
      digitalWrite(greenLedPin, LOW);
      securityArmed = false;
      counter = 0;
      LastAccess++;
    }
    else
    {
      Serial.println("Access denied.");
      digitalWrite(redLedPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      delay(1000);
      digitalWrite(redLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
      counter++;
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
