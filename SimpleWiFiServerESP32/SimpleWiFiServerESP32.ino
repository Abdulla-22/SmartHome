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
HardwareSerial ArduinoSerial(2); // Define Arduino Serial object

// Variables for lighting control
bool outdoorLightAuto = true;
bool indoorLightAuto = true;
bool SecurityMood = false;
int outdoorLightState = 0;
int indoorLightState = 0;
int EnvironmentSystemAuto = 1;
int EnvironmentSystemAC = 1;

// Function prototypes
void initializeWiFi();
void initializeFirebase();
void sendCommandToArduino(const String &command);
void handleLightingSystem();
void logSensorDataFromArduino();

void setup()
{
  // Initialize Serial communication
  Serial.begin(115200);
  ArduinoSerial.begin(9600, SERIAL_8N1, 16, 17); // Use GPIO16 (RX) and GPIO17 (TX)

  // Initialize Wi-Fi and Firebase
  initializeWiFi();
  initializeFirebase();

  Serial.println("Setup complete.");
}

void loop()
{

  // Handle lighting system logic
  handleLightingSystem();

  // Log sensor data from Arduino
  logSensorDataFromArduino();
}

/* Function Definitions */

// Initialize Wi-Fi connection
void initializeWiFi()
{
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

// Initialize Firebase connection
void initializeFirebase()
{
  config.api_key = API_KEY;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  config.database_url = DATABASE_URL;
  Firebase.begin(&config, &auth);

  Serial.println("Firebase initialized");
}

// Send a command to Arduino
void sendCommandToArduino(const String &command)
{
  ArduinoSerial.println(command);                // Send the command to Arduino
  Serial.println("Sent to Arduino: " + command); // Debug log
}

// Handle lighting system logic
void handleLightingSystem()
{
  // Get outdoor lighting auto-control state
  if (Firebase.RTDB.getBool(&fbdo, "/LightingSystem/Outdoor/Auto", &outdoorLightAuto))
  {
    sendCommandToArduino(outdoorLightAuto ? "OUT_DOOR_LIGHT_AUTO:1" : "OUT_DOOR_LIGHT_AUTO:0");
    // Serial.println(outdoorLightAuto);
  }

  if (Firebase.RTDB.getBool(&fbdo, "/LightingSystem/Indoor/Auto", &indoorLightAuto))
  {
    sendCommandToArduino(indoorLightAuto ? "IN_DOOR_LIGHT_AUTO:1" : "IN_DOOR_LIGHT_AUTO:0");
    // Serial.println(outdoorLightAuto);
  }

  // Get outdoor lighting state
  if (Firebase.RTDB.getInt(&fbdo, "/LightingSystem/Outdoor/State", &outdoorLightState))
  {
    sendCommandToArduino("OUT_DOOR_LIGHT_STATE:" + String(outdoorLightState));
    // Serial.println("OUT D:"+outdoorLightAuto);
  }

  // Get indoor lighting state
  if (Firebase.RTDB.getInt(&fbdo, "/LightingSystem/Indoor/State", &indoorLightState))
  {
    sendCommandToArduino("IN_DOOR_LIGHT_STATE:" + String(indoorLightState));
    // Serial.println("IN D:"+outdoorLightAuto);
  }

  if (Firebase.RTDB.getInt(&fbdo, "/SecuritySystem/Armed", &SecurityMood)) 
  {
    sendCommandToArduino("SECURITY_ARMED:" + String(SecurityMood));
  }

  if (Firebase.RTDB.getInt(&fbdo, "/EnvironmentSystem/Auto", &EnvironmentSystemAuto)) 
  {
    sendCommandToArduino("ENVIRONMENT_AUTO_MODE:" + String(EnvironmentSystemAuto));
  }

  if (Firebase.RTDB.getInt(&fbdo, "/EnvironmentSystem/ac", &EnvironmentSystemAC)) 
  {
    sendCommandToArduino("ENVIRONMENT_AC_MODE:" + String(EnvironmentSystemAC));
  }
}

// Log sensor data from Arduino
void logSensorDataFromArduino()
{
  if (ArduinoSerial.available() >= 1)
  {
    String incomingData = ArduinoSerial.readStringUntil('\n');

    // Handle soil moisture data
    if (incomingData.startsWith("SOIL_MOISTURE:"))
    {
      int soilMoisture = incomingData.substring(14).toInt();
      if (Firebase.RTDB.setInt(&fbdo, "/GardenSystem/SoilMoisture", soilMoisture))
      {
        Serial.println("Soil Moisture logged: " + String(soilMoisture));
      }
      else
      {
        Serial.println("Failed to log Soil Moisture: " + fbdo.errorReason());
      }
    }
    // Handle rain sensor data
    else if (incomingData.startsWith("RAIN_SENSOR:"))
    {
      // Extract the rain sensor value from the incoming data
      int rainValue = incomingData.substring(12).toInt();
      Serial.println("My rain val: " + String(rainValue));

      // Convert the digital rain sensor value to a boolean
      bool rainDetected = (rainValue == 0); // Assuming 0 means rain detected

      // Log the boolean value to Firebase
      if (Firebase.RTDB.setBool(&fbdo, "/GardenSystem/Rain", rainDetected))
      {
        Serial.println("Rain logged: " + String(rainDetected ? "True" : "False"));
      }
      else
      {
        Serial.println("Failed to log Rain: " + fbdo.errorReason());
      }
    } 
    else if (incomingData.startsWith("SECURITY:")) 
    {
      // Extract the alarm status
      int AlarmVal = incomingData.substring(9).toInt();
      Serial.println("My alarm val: " + String(AlarmVal));

      // Log the boolean value to Firebase
      if (Firebase.RTDB.setBool(&fbdo, "/SecuritySystem/Alarm", (AlarmVal == 1)))
      {
        Serial.println("Alarm logged: " + String((AlarmVal == 1) ? "True" : "False"));
      }
      else
      {
        Serial.println("Failed to log Alarm: " + fbdo.errorReason());
      }
    }
    // else if (the AC) 
    // {

    // }
  }
}
