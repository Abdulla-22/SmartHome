String command = ""; // Store the incoming command

const int ledPin = 9; // LED connected to pin 9
const int sensorPin = A0; // Example sensor connected to analog pin A0
unsigned long lastSensorSendTime = 0; // Timer for periodic sensor data
const unsigned long sensorInterval = 1000; // Interval to send sensor data (1 second)

void setup() {
  Serial.begin(9600); // Initialize Serial communication with ESP32
  pinMode(ledPin, OUTPUT);
  pinMode(sensorPin, INPUT);
  Serial.println("Arduino is ready!");
}

void loop() {
  // Check for commands from ESP32
  if (Serial.available()) {
    command = Serial.readStringUntil('\n'); // Read the incoming command
    command.trim(); // Remove any extra whitespace

    if (command == "IN_DOOR_LED_ON") {
      digitalWrite(ledPin, HIGH); // Turn the LED on
      Serial.println("LED is ON");
    } else if (command == "IN_DOOR_LED_OFF") {
      digitalWrite(ledPin, LOW); // Turn the LED off
      Serial.println("LED is OFF");
    } else {
      Serial.println("Invalid Command: " + command);
    }
  }

  // Send sensor data to ESP32 periodically
  // unsigned long currentTime = millis();
  // if (currentTime - lastSensorSendTime >= sensorInterval) {
  //   lastSensorSendTime = currentTime;

  //   int sensorValue = analogRead(sensorPin); // Read sensor value
  //   Serial.println("SENSOR:" + String(sensorValue)); // Send sensor data
  // }
}
