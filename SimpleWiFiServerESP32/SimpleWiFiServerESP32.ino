#include <FireBase32.h>      // Include the FireBase32 library for Firebase operations
#include <WiFi.h>            // Include the WiFi library for ESP32 WiFi functionality
#include <HardwareSerial.h>  // Include the HardwareSerial library for UART communication

// Firebase Configuration: Replace with your Firebase Realtime Database URL and API Key
FireBase32 db("https://smatrhome-default-rtdb.firebaseio.com/", "AIzaSyAfLEWczY84xV9KDQbFKAugAIXrBwKeP_w");

// UART Configuration: Use GPIO16 (RX) and GPIO17 (TX) for communication with the Arduino
HardwareSerial MySerial(2);

// Pin Definitions
const int ledPin = 2; // ESP32 built-in LED pin for feedback
String command = "";  // String to store incoming commands

// Sensor Data
int sensorData = 0; // Placeholder for sensor data received from the Arduino

void setup() {
  // Start debugging Serial communication
  Serial.begin(115200);

  // Initialize UART communication with Arduino
  MySerial.begin(9600, SERIAL_8N1, 16, 17); // Baud rate: 9600, 8 data bits, no parity, 1 stop bit

  // Connect to WiFi using the FireBase32 library
  db.wifi("Zain_H122_CB1E", "M6GJ2QEF53J"); // Replace with your WiFi SSID and Password

  // Check Firebase connection status

  // Initialize the built-in LED pin for feedback
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW); // Turn off the LED initially
}

void loop() {
  // Handle commands from Firebase
  handleFirebaseCommands();

  // Sync sensor data from Arduino to Firebase
  syncArduinoDataToFirebase();

  delay(100); // Add a delay to avoid spamming the loop
}

// Function to handle commands from Firebase
void handleFirebaseCommands() {
  // Retrieve the command from the Firebase path `/control/command`
  String firebaseCommand = "";
  db.GetData("/control/command", &firebaseCommand);

  // If a command is found
  if (firebaseCommand.length() > 0) {
    Serial.println("Firebase Command: " + firebaseCommand); // Print the command
    MySerial.println(firebaseCommand); // Forward the command to the Arduino via UART

    // Handle the command locally (e.g., control an LED)
    if (firebaseCommand == "LED_ON") {
      digitalWrite(ledPin, HIGH); // Turn on the built-in LED
    } else if (firebaseCommand == "LED_OFF") {
      digitalWrite(ledPin, LOW); // Turn off the built-in LED
    }
  }
}

// Function to sync data from Arduino to Firebase
void syncArduinoDataToFirebase() {
  // Check if there is data available from the Arduino
  if (MySerial.available()) {
    // Read the incoming data from the Arduino
    String receivedData = MySerial.readStringUntil('\n');
    receivedData.trim(); // Remove any extra whitespace

    // Write the received data to the Firebase path `/sensor/data`
    //db.WriteData(receivedData.toInt(), "/sensor/data");
    Serial.println("Sensor Data Synced: " + receivedData); // Print the synced data
  }
}
