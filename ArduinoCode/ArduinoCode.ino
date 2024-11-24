String command = ""; // Store the incoming command

const int ledPin = 9; // LED connected to pin 9
const int sensorPin = A0; // Example sensor connected to analog pin A0

void setup() {
  Serial.begin(9600); // Initialize Serial communication with ESP32
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);
}

void loop() {
  // Check for commands from ESP32
  if (Serial.available()) {
    command = Serial.readStringUntil('\n'); // Read the incoming command
    command.trim(); // Remove any extra whitespace

    if (command == "LED_ON") {
      digitalWrite(ledPin, HIGH); // Turn the LED on
      Serial.println("LED is ON");
    } else if (command == "LED_OFF") {
      digitalWrite(ledPin, LOW); // Turn the LED off
      Serial.println("LED is OFF");
    }
  }

  // Send sensor data to ESP32
  int sensorValue = analogRead(sensorPin);
  Serial.println("SENSOR:" + String(sensorValue));
  delay(1000); // Adjust delay as needed
}
