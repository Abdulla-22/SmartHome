#include <WiFi.h>
#include <Firebase_ESP_Client.h>

/* Wi-Fi and Firebase credentials */
#define WIFI_SSID "Zain_H122_CB1E"
#define WIFI_PASSWORD "M6GJ2QEF53J"
#define API_KEY "AIzaSyB0S9NtzkxVAHmU3A-7zLnE3-yTbrPB4dY"
#define DATABASE_URL "https://espcontroller-dc45b-default-rtdb.firebaseio.com/"
#define USER_EMAIL "abdulla38898@gmail.com"
#define USER_PASSWORD "12345678"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Serial communication
HardwareSerial ArduinoSerial(2);  // Define Arduino Serial object

// Variables for lighting control
bool outdoorLightAuto = true;
int outdoorLightState = 0;
int indoorLightState = 0;

// Function prototypes
void initializeWiFi();
void initializeFirebase();
void sendCommandToArduino(const String &command);
void handleLightingSystem();
void logSensorDataFromArduino();

void setup() {
  // Initialize Serial communication
  Serial.begin(115200);
  ArduinoSerial.begin(9600, SERIAL_8N1, 16, 17);  // Use GPIO16 (RX) and GPIO17 (TX)

  // Initialize Wi-Fi and Firebase
  initializeWiFi();
  initializeFirebase();

  Serial.println("Setup complete.");
}

void loop() {
  if (Firebase.ready()) {
    // Handle lighting system logic
    handleLightingSystem();

    // Log sensor data from Arduino
    logSensorDataFromArduino();
  }

  delay(300);  // Delay for smoother operation
}

/* Function Definitions */

// Initialize Wi-Fi connection
void initializeWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

// Initialize Firebase connection
void initializeFirebase() {
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.begin(&config, &auth);

  Serial.println("Firebase initialized");
}

// Send a command to Arduino
void sendCommandToArduino(const String &command) {
  ArduinoSerial.println(command);                 // Send the command to Arduino
  Serial.println("Sent to Arduino: " + command);  // Debug log
}

// Handle lighting system logic
void handleLightingSystem() {
  // Get outdoor lighting auto-control state
  if (Firebase.RTDB.getBool(&fbdo, "/LightingSystem/Outdoor/Auto", &outdoorLightAuto)) {
    sendCommandToArduino(outdoorLightAuto ? "OUT_DOOR_LIGHT_AUTO:1" : "OUT_DOOR_LIGHT_AUTO:0");
  }

  // Get outdoor lighting state
  if (Firebase.RTDB.getInt(&fbdo, "/LightingSystem/Outdoor/State", &outdoorLightState)) {
    sendCommandToArduino("OUT_DOOR_LIGHT_STATE:" + String(outdoorLightState));
  }

  // Get indoor lighting state
  if (Firebase.RTDB.getInt(&fbdo, "/LightingSystem/Indoor/State", &indoorLightState)) {
    sendCommandToArduino("IN_DOOR_LIGHT_STATE:" + String(indoorLightState));
  }
}

// Log sensor data from Arduino
void logSensorDataFromArduino() {
  while (ArduinoSerial.available()) {
    String incomingData = ArduinoSerial.readStringUntil('\n');

    if (incomingData.startsWith("TEMP:")) {
      int temp = incomingData.substring(5).toInt();
      Firebase.RTDB.setInt(&fbdo, "/GardenSystem/Temperature", temp);
      Serial.println("Temperature logged: " + String(temp));
    } else if (incomingData.startsWith("HUMIDITY:")) {
      int humidity = incomingData.substring(9).toInt();
      Firebase.RTDB.setInt(&fbdo, "/GardenSystem/Humidity", humidity);
      Serial.println("Humidity logged: " + String(humidity));
    }
  }
}
