String command = ""; // Store the incoming command

const int ledPin = 9;     // LED connected to pin 9
const int sensorPin = A0; // Example sensor connected to analog pin A0

void setup() {
  Serial.begin(9600); // Initialize Serial communication with ESP32
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);
}

void loop() {
  // Check if a command is available from the ESP32
  if (Serial.available()) {
    command = Serial.readStringUntil('\n'); // Read the incoming command
    command.trim(); // Remove any extra whitespace

    // Handle the command
    if (command == "LED_ON") {
      digitalWrite(ledPin, HIGH); // Turn the LED on
      Serial.println("LED is ON"); // Send confirmation back to ESP32
    } else if (command == "LED_OFF") {
      digitalWrite(ledPin, LOW); // Turn the LED off
      Serial.println("LED is OFF"); // Send confirmation back to ESP32
    } else if (command == "GET_SENSOR") {
      int sensorValue = analogRead(sensorPin); // Read the sensor value
      Serial.println("SENSOR_VALUE:" + String(sensorValue)); // Send sensor data to ESP32
    }
  }
}
