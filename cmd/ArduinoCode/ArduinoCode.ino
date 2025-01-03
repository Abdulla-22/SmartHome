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
bool armedStatus = false; // Local flag for armed mode
bool prevSecurityArmed = false;
bool securityMode = false;  // Database-controlled flag
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

  Serial.println("System Initialized.");
}

void loop()
{
  char key = keypad.getKey();
  if (key)
    handleKeypadInput(key);

  handleSecuritySystem();

  if (Serial.available())
{
    Serial.println("Data available on Serial."); // Debug message
    String command = Serial.readStringUntil('\n');
    Serial.print("Received command: ");
    Serial.println(command);
    processESP32Command(command);
}
}

// Function to handle security system
void handleSecuritySystem()
{
  if (counter >= 3)
  {
    armedStatus = true;
  }

  if (armedStatus) // Only check motion when armed
  {
    int motionDetected = digitalRead(pirSensorPin);
    int vibrationDetected = digitalRead(vibrationSensorPin);

    if (motionDetected || vibrationDetected)
    {
      digitalWrite(redLedPin, HIGH);
      digitalWrite(buzzerPin, HIGH);
      
      // sendToESP32("SECURITY:1");

      Serial.println("Alarm is on"); // Print the alarm status
    }
    else
    {
      digitalWrite(redLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
      
    }

}

// Check for changes in securityArmed and send only if changed
  if (armedStatus != prevSecurityArmed)
  {
    sendToESP32(armedStatus ? "SECURITY_ARMED:1" : "SECURITY_ARMED:0");
    prevSecurityArmed = armedStatus; // Update the previous state
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
      armedStatus = false;
      counter = 0; // Reset counter on successful entry
      LastAccess++;
    }
    else
    {
      counter++; // Increment the counter for wrong attempts
      Serial.println("Access denied.");
      digitalWrite(buzzerPin, HIGH);
      digitalWrite(redLedPin, HIGH);
      delay(1000);
      digitalWrite(redLedPin, LOW);
      digitalWrite(buzzerPin, LOW);
    }

    enteredPassword = ""; // Clear the entered password
  }
  else if (key == '*')
  {
    enteredPassword = ""; // Clear the entered password
  }
  else
  {
    enteredPassword += key; // Append the pressed key to the password
    Serial.println("Password so far: " + enteredPassword);
  }
}

void processESP32Command(String command)
{
  command.trim(); // Remove any extra spaces or newline characters
  if (command.startsWith("SECURITY_MODE:"))
  {
    String modeStr = command.substring(14); // Extract the value after "SECURITY_MODE:"
    int mode = modeStr.toInt(); // Convert the extracted value to an integer
    armedStatus = (mode == 1); // Update security mode
    Serial.print("Security mode updated: ");
    Serial.println(armedStatus ? "Armed" : "Disarmed");
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


