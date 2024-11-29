#include <Keypad.h>
#include <Servo.h>

// Define your keypad layout
char key[4][4] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rpin[4] = {13, 12, 11, 10};
byte cpin[4] = {9, 8, 7, 6};
Keypad keypad = Keypad(makeKeymap(key), rpin, cpin, 4, 4);

// LED pins
int redled = 5;
int greenled = 4;

// Password setup
const String password = "1234"; // Correct password
String input = ""; // Stores user input

void setup() {
  pinMode(redled, OUTPUT);
  pinMode(greenled, OUTPUT);

  // Turn on the red LED at the start
  digitalWrite(redled, HIGH);
  digitalWrite(greenled, LOW);

  Serial.begin(9600); // For debugging
}

void loop() {
  char key = keypad.getKey(); // Read the keypad

  if (key) { // If a key is pressed
    Serial.println(key); // For debugging

    if (key == '#') { 
      // '#' is used to submit the input
      if (input == password) {
        // Password is correct
        digitalWrite(redled, LOW);  // Turn off red LED
        digitalWrite(greenled, HIGH); // Turn on green LED
        delay(5000);                // Wait for 5 seconds
        digitalWrite(greenled, LOW); // Turn off green LED
        digitalWrite(redled, HIGH);  // Turn red LED back on
      } else {
        // Password is incorrect
        Serial.println("Incorrect Password"); // Debugging
      }
      input = ""; // Reset input
    } else if (key == '*') {
      // '*' is used to clear the input
      input = "";
      Serial.println("Input cleared"); // Debugging
    } else {
      // Add the key to the input
      input += key;
    }
  }
}
