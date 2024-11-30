// Define pin connections
const int soundSensorPin = 2; // Pin connected to the digital output of the sound sensor
const int ledPin = 13;        // Pin connected to the LED

void setup() {
  pinMode(soundSensorPin, INPUT); // Set the sound sensor pin as input
  pinMode(ledPin, OUTPUT);        // Set the LED pin as output

  // Initialize serial communication for debugging
  Serial.begin(9600);
  Serial.println("Sound Sensor Test");
}

void loop() {
  // Read the digital signal from the sound sensor
  int soundDetected = digitalRead(soundSensorPin);

  if (soundDetected == HIGH) {
    // Sound detected, turn on LED
    digitalWrite(ledPin, HIGH);
    Serial.println("Sound detected!");
  } else {
    // No sound detected, turn off LED
    digitalWrite(ledPin, LOW);
  }

  // Small delay to stabilize readings
  delay(100);
}
