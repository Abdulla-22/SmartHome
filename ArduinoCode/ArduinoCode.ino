// Pin definitions for Lighting System
const int outdoorLedPin = 3;     // Outdoor LED pin
const int indoorLedPin = 5;      // Indoor LED pin
const int ldrPin = A0;           // LDR sensor pin
const int soundSensorPin = A1;   // Analog pin connected to sound sensor

// Constants
const unsigned long clapDelay = 500; // Minimum delay between claps (in milliseconds)
const int ldrThreshold = 500;       // Threshold for LDR sensor

// Variables for states
unsigned long lastClapTime = 0;     // Time of the last detected clap
bool outdoorLightAuto = true;       // Automatic control for outdoor lighting
bool indoorLightState = false;      // Indoor light state (on/off)
bool outdoorLightState = false;     // Outdoor light state (on/off)

void setup() {
  Serial.begin(9600);                 // Start Serial communication with ESP32
  pinMode(outdoorLedPin, OUTPUT);
  pinMode(indoorLedPin, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(soundSensorPin, INPUT);     // Set sound sensor pin as input

  // Turn off LEDs at startup
  digitalWrite(outdoorLedPin, LOW);
  digitalWrite(indoorLedPin, LOW);

  Serial.println("Arduino is ready.");
}

void loop() {
  // Check if data is available from ESP32
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');  // Read the command
    command.trim();                                 // Remove extra whitespace
    processESP32Data(command);                     // Handle the command
  }

  // Handle lighting system
  handleOutdoorLighting();
  handleIndoorLighting();

  delay(100);
}

// Function to parse and handle incoming ESP32 data
void processESP32Data(String command) {
  int delimiterIndex = command.indexOf(':');
  if (delimiterIndex != -1) {
    String tag = command.substring(0, delimiterIndex);
    String value = command.substring(delimiterIndex + 1);
    int intValue = value.toInt();  // Convert value to integer

    if (tag == "IN_DOOR_LIGHT_STATE") {
      handleIndoorState(intValue);
    } else if (tag == "OUT_DOOR_LIGHT_AUTO") {
      handleOutdoorAutoState(intValue);
    } else if (tag == "OUT_DOOR_LIGHT_STATE") {
      handleOutdoorState(intValue);
    } else {
      Serial.println("Unknown tag: " + tag);
    }
  } else {
    Serial.println("Invalid command format: " + command);
  }
}

// Handle indoor light state
void handleIndoorState(int state) {
  indoorLightState = (state == 1);
  Serial.println("Indoor Light State: " + String(indoorLightState));
}

// Handle outdoor light auto/manual mode
void handleOutdoorAutoState(int state) {
  outdoorLightAuto = (state == 1);
  Serial.println(outdoorLightAuto ? "Outdoor light set to AUTO mode." : "Outdoor light set to MANUAL mode.");
}

// Handle outdoor light state
void handleOutdoorState(int state) {
  outdoorLightState = (state == 1);
  Serial.println("Outdoor Light State: " + String(outdoorLightState));
}

// Send data to ESP32
void sendDataToESP(const String &data) {
  Serial.println(data);
}

// Control outdoor lighting
void handleOutdoorLighting() {
  if (outdoorLightAuto) {
    // Automatic control mode
    int ldrValue = analogRead(ldrPin);  // Read LDR value
    // Serial.println("LDR Value: " + String(ldrValue));  // Debugging
    digitalWrite(outdoorLedPin, (ldrValue < ldrThreshold) ? HIGH : LOW);
  } else {
    // Manual control mode
    digitalWrite(outdoorLedPin, outdoorLightState ? HIGH : LOW);
  }
}

// Control indoor lighting with sound sensor
void handleIndoorLighting() {
  int soundDetected = analogRead(soundSensorPin);  // Read the sound sensor
  // Serial.println("Sound Detected: " + String(soundDetected));

  if (soundDetected > 30) {
    unsigned long currentTime = millis();
    if (currentTime - lastClapTime > clapDelay) {  // Ensure delay between claps
      indoorLightState = !indoorLightState;        // Toggle the LED state
      lastClapTime = currentTime;                  // Update the last clap time
    }
  }

  digitalWrite(indoorLedPin, indoorLightState ? HIGH : LOW);
}
