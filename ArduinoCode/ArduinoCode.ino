String command = ""; // Store the incoming command

int OutDoorLED = 8;
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

    Serial.print("The received command: ");
    Serial.println(command);

    // Use switch for command handling
    switch (getCommandType(command)) {
      case 1: // LED_ON
        digitalWrite(ledPin, HIGH); // Turn the LED on
        Serial.println("LED is ON"); // Send confirmation back to ESP32
        break;

      case 2: // LED_OFF
        digitalWrite(ledPin, LOW); // Turn the LED off
        Serial.println("LED is OFF"); // Send confirmation back to ESP32
        break;

      case 3: // GET_SENSOR
        int sensorValue = analogRead(sensorPin); // Read the sensor value
        Serial.println("SENSOR_VALUE:" + String(sensorValue)); // Send sensor data to ESP32
        break;

      default:
        Serial.println("Invalid command"); // Handle unrecognized commands
        break;
    }
  }
}

// Function to determine the command type
int getCommandType(String cmd) {
  if (cmd == "LED_ON") {
    return 1;
  } else if (cmd == "LED_OFF") {
    return 2;
  } else if (cmd == "GET_SENSOR") {
    return 3;
  } else {
    return 0; // Invalid command
  }
}
