#include <WiFi.h>
#include <Firebase_ESP_Client.h>

// Firebase and WiFi configuration
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// WiFi Credentials
const char* ssid = ""; // wifi name 
const char* password = ""; //wifi password

// Firebase Configuration
const char* firebase_host = "smatrhome-default-rtdb.firebaseio.com";
const char* api_key = ""; //Add the api key

// Pin Definitions
const int ledPin = 2; // ESP32 built-in LED pin for feedback

void setup() {
  Serial.begin(115200);

  // WiFi setup
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi.");
  Serial.println("IP Address: " + WiFi.localIP().toString());

  // Firebase setup
  config.host = firebase_host;
  config.api_key = api_key;

  // Enable anonymous authentication
  auth.user.email = "";
  auth.user.password = "";

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Setup LED pin
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);
}

void loop() {
  handleFirebaseCommands();
  delay(100);
}

void handleFirebaseCommands() {
  if (Firebase.RTDB.getString(&fbdo, "/control/command")) {
    String command = fbdo.stringData();
    Serial.println("Command from Firebase: " + command);

    if (command == "LED_ON") {
      digitalWrite(ledPin, HIGH);
      Serial.println("LED turned ON.");
    } else if (command == "LED_OFF") {
      digitalWrite(ledPin, LOW);
      Serial.println("LED turned OFF.");
    } else {
      Serial.println("Unknown command.");
    }
  } else {
    Serial.println("Failed to get command from Firebase: " + fbdo.errorReason());
  }
}
