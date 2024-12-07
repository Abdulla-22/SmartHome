#include <DHT.h>
#include <DHT_U.h>

#include <WiFi.h>
#include <Firebase_ESP_Client.h>

#define DHTPIN 4      // Pin connected to the DHT11 sensor
#define DHTTYPE DHT11 // Define the type of DHT sensor

/* Wi-Fi and Firebase credentials */
#define WIFI_SSID "Zain_H122_CB1E"                                              // Replace with your Wi-Fi SSID
#define WIFI_PASSWORD "M6GJ2QEF53J"                                             // Replace with your Wi-Fi Password
#define API_KEY "AIzaSyB0S9NtzkxVAHmU3A-7zLnE3-yTbrPB4dY"                       // Replace with your Firebase API Key
#define DATABASE_URL "https://espcontroller-dc45b-default-rtdb.firebaseio.com/" // Replace with your Firebase Database URL
#define USER_EMAIL "abdulla38898@gmail.com"
#define USER_PASSWORD "12345678"

// Firebase objects for communication with the database
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Serial communication with Arduino
HardwareSerial ArduinoSerial(2); // Serial2 (ESP32 UART communication)

DHT dht(DHTPIN, DHTTYPE);        // Initialize the DHT sensor
bool environmentAutoMode = true; // Automatic environment control mode
const int tempThreshold = 30;    // Temperature threshold for AC in °C
bool acBOOL = false;             // Manual AC control state
const int acGreenLedPin = 14;    // Green LED for AC ON
const int acRedLedPin = 12;      // Red LED for AC OFF

// Sensor pins connected to ESP32
const int ldrPin = 34;         // LDR sensor for outdoor lighting
const int outdoorLedPin = 25;  // Outdoor LED
const int indoorLedPin = 26;   // Indoor LED
const int soundSensorPin = 35; // Sound sensor for indoor lighting

const int waterPumpPin = 27;    // LED to simulate water pump
const int soilMoisturePin = 32; // Soil moisture sensor pin
const int rainSensorPin = 33;   // Rain sensor pin (digital)

// System variables
bool outdoorLightAuto = true;  // Outdoor lighting auto/manual mode
bool indoorLightAuto = true;   // Indoor lighting auto/manual mode
int outdoorLightState = false; // Outdoor lighting state (on/off)
int indoorLightState = false;  // Indoor lighting state (on/off)
const int ldrThreshold = 500;  // Threshold for LDR sensor (adjust as needed)
const int soundThreshold = 25; // Threshold for sound sensor (adjust as needed)

const int moistureThreshold = 2800;
bool WaterPump = false;
bool gardenAutoMode = true;

void setup()
{
  Serial.begin(115200);                          // Debugging
  ArduinoSerial.begin(9600, SERIAL_8N1, 16, 17); // Communication with Arduino

  dht.begin(); // Initialize DHT11 sensor
  pinMode(acGreenLedPin, OUTPUT);
  pinMode(acRedLedPin, OUTPUT);

  // Turn off LEDs initially
  digitalWrite(acGreenLedPin, LOW);
  digitalWrite(acRedLedPin, LOW);

  pinMode(outdoorLedPin, OUTPUT);
  pinMode(indoorLedPin, OUTPUT);

  pinMode(waterPumpPin, OUTPUT);
  pinMode(soilMoisturePin, INPUT);
  pinMode(rainSensorPin, INPUT);

  pinMode(acGreenLedPin, OUTPUT);
  pinMode(acRedLedPin, OUTPUT);

  // Turn off LEDs initially
  digitalWrite(acGreenLedPin, LOW);
  digitalWrite(acRedLedPin, LOW);

  digitalWrite(waterPumpPin, LOW); // Ensure the LED is off initially

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(300);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");

  // Initialize Firebase
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;
  Firebase.begin(&config, &auth);
}

void loop()
{
  // Read sensor data and log to Firebase
  // readAndLogSensors();
  readDB();

  handleLightingSystem();

  handleGardenSystem();

  handleEnvironmentControl();

  // Handle communication with Arduino
  handleArduinoCommunication();
}

// Function to read sensor data and log it to Firebase (not used)
void readAndLogSensors()
{
  int ldrValue = analogRead(ldrPin);                   // Read LDR value
  int soundValue = analogRead(soundSensorPin);         // Read sound sensor value
  int soilMoistureValue = analogRead(soilMoisturePin); // Read soil moisture value
  int rainValue = digitalRead(rainSensorPin);          // Read rain sensor state (digital)

  // Log data to Firebase
  Firebase.RTDB.setInt(&fbdo, "/LightingSystem/LDR", ldrValue);
  Firebase.RTDB.setInt(&fbdo, "/Environment/Sound", soundValue);
  Firebase.RTDB.setInt(&fbdo, "/GardenSystem/SoilMoisture", soilMoistureValue);
  Firebase.RTDB.setBool(&fbdo, "/GardenSystem/Rain", rainValue == 0); // Log rain status as boolean
}

void readDB()
{
  Firebase.RTDB.getBool(&fbdo, "/LightingSystem/Outdoor/Auto", &outdoorLightAuto);
  Firebase.RTDB.getBool(&fbdo, "/LightingSystem/Indoor/Auto", &indoorLightAuto);
  Firebase.RTDB.getInt(&fbdo, "/LightingSystem/Outdoor/State", &outdoorLightState);
  Firebase.RTDB.getInt(&fbdo, "/LightingSystem/Indoor/State", &indoorLightState);

  Firebase.RTDB.getBool(&fbdo, "/GardenSystem/WaterPump", &WaterPump);
  Firebase.RTDB.getBool(&fbdo, "/GardenSystem/Auto", &gardenAutoMode);

  Firebase.RTDB.getBool(&fbdo, "/EnvironmentSystem/Auto", &environmentAutoMode);
  Firebase.RTDB.getBool(&fbdo, "/EnvironmentSystem/ManualAC", &acBOOL);
}

void handleLightingSystem()
{
  // Outdoor lighting control
  if (outdoorLightAuto)
  {
    int ldrValue = analogRead(ldrPin); // Read LDR sensor
    // Serial.println(ldrValue);
    if (ldrValue < ldrThreshold)
    {
      digitalWrite(outdoorLedPin, HIGH); // Turn on outdoor light
      // Serial.println("HIGH");
      outdoorLightState = 1;
    }
    else
    {
      digitalWrite(outdoorLedPin, LOW); // Turn off outdoor light
      // Serial.println("LOW");
      outdoorLightState = 0;
    }
    Firebase.RTDB.setInt(&fbdo, "/LightingSystem/Outdoor/State", outdoorLightState);
  }
  else
  {
    if (outdoorLightState == 1)
    {
      digitalWrite(outdoorLedPin, HIGH);
    }
    else
    {
      digitalWrite(outdoorLedPin, LOW);
    }
  }

  // Indoor lighting control
  if (indoorLightAuto)
  {
    int soundValue = analogRead(soundSensorPin); // Read sound sensor
    Serial.println(String(soundValue) + ">" + String(soundThreshold));
    if (soundValue > soundThreshold)
    {
      // Toggle indoor light on clap
      if (indoorLightState == 1)
      {
        indoorLightState = 0;
      }
      else if (indoorLightState == 0)
      {
        indoorLightState = 1;
      }
      delay(500); // Debounce delay
    }
    Firebase.RTDB.setInt(&fbdo, "/LightingSystem/Indoor/State", indoorLightState);

    if (indoorLightState == 1)
    {
      digitalWrite(indoorLedPin, HIGH);
    }
    else
    {
      digitalWrite(indoorLedPin, LOW);
    }
  }
  else
  {

    if (indoorLightState == 1)
    {
      digitalWrite(indoorLedPin, HIGH);
    }
    else
    {
      digitalWrite(indoorLedPin, LOW);
    }
  }
}

void handleGardenSystem()
{
  int soilMoistureValue = analogRead(soilMoisturePin); // Read soil moisture value
  Serial.println("soilMoistureValue= " + String(soilMoistureValue));

  int rainSensorState = digitalRead(rainSensorPin); // Read rain sensor state (digital)
  Serial.println("rainSensorState " + String(rainSensorState));

  // Garden system control logic
  if (gardenAutoMode)
  {
    if (rainSensorState == LOW)
    {
      // Rain detected; turn off water pump (LED OFF)
      digitalWrite(waterPumpPin, LOW);
    }
    else if (soilMoistureValue > moistureThreshold)
    {
      // Soil is dry; turn on water pump (LED ON)
      digitalWrite(waterPumpPin, HIGH);
    }
    else
    {
      // Soil moisture is adequate; turn off water pump (LED OFF)
      digitalWrite(waterPumpPin, LOW);
    }
  }
  else
  {
    Firebase.RTDB.getBool(&fbdo, "/GardenSystem/WaterPump", &WaterPump);

    if (WaterPump)
    {
      digitalWrite(waterPumpPin, HIGH);
    }
    else
    {
      digitalWrite(waterPumpPin, LOW);
    }
  }

  // Log data to Firebase
  Firebase.RTDB.setInt(&fbdo, "/GardenSystem/SoilMoisture", soilMoistureValue);
  Firebase.RTDB.setBool(&fbdo, "/GardenSystem/Rain", rainSensorState == 0);                   // Rain detected is true when LOW
  Firebase.RTDB.setBool(&fbdo, "/GardenSystem/WaterPump", digitalRead(waterPumpPin) == HIGH); // Log LED state
}

void handleEnvironmentControl()
{
  if (environmentAutoMode)
  {
    // Read temperature and humidity from DHT11
    float temperature = dht.readTemperature(); // Read temperature in Celsius
    float humidity = dht.readHumidity();       // Read humidity

    // Check if readings are valid
    if (isnan(temperature) || isnan(humidity))
    {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }

    // Debugging
    Serial.println("Temperature: " + String(temperature) + "°C");
    Serial.println("Humidity: " + String(humidity) + "%");

    // Control AC based on temperature threshold
    if (temperature > tempThreshold)
    {
      digitalWrite(acGreenLedPin, HIGH); // Turn on the green LED (AC ON)
      digitalWrite(acRedLedPin, LOW);    // Turn off the red LED (AC OFF)
      Serial.println("Air Conditioner ON.");
    }
    else
    {
      digitalWrite(acGreenLedPin, LOW); // Turn off the green LED (AC OFF)
      digitalWrite(acRedLedPin, HIGH);  // Turn on the red LED (AC OFF)
      Serial.println("Air Conditioner OFF.");
    }

    // Log temperature and humidity to Firebase
    if (!Firebase.RTDB.setFloat(&fbdo, "/EnvironmentSystem/Temperature", temperature))
    {
      Serial.println("Failed to log Temperature: " + fbdo.errorReason());
    }
    if (!Firebase.RTDB.setFloat(&fbdo, "/EnvironmentSystem/Humidity", humidity))
    {
      Serial.println("Failed to log Humidity: " + fbdo.errorReason());
    }
  }
  else
  {
    // Manual AC control
    digitalWrite(acGreenLedPin, acBOOL ? HIGH : LOW); // AC ON if acBOOL is true
    digitalWrite(acRedLedPin, acBOOL ? LOW : HIGH);   // AC OFF if acBOOL is true
    Serial.println("Manual AC Control: " + String(acBOOL ? "ON" : "OFF"));
  }

  // Log AC state to Firebase
  if (!Firebase.RTDB.setBool(&fbdo, "/EnvironmentSystem/ACState", digitalRead(acGreenLedPin) == HIGH))
  {
    Serial.println("Failed to log AC State: " + fbdo.errorReason());
  }
}

// Function to handle communication with Arduino
void handleArduinoCommunication()
{
  if (ArduinoSerial.available())
  {
    // Read incoming command from Arduino
    String command = ArduinoSerial.readStringUntil('\n');
    processArduinoCommand(command); // Process the command
  }
}

// Function to process commands from Arduino
void processArduinoCommand(String command)
{
  if (command.startsWith("BASKET_DISTANCE"))
  {
    // Log the garbage basket distance to Firebase
    int distance = command.substring(16).toInt();
    Firebase.RTDB.setInt(&fbdo, "/GarbageBasket/Distance", distance);
  }
  else if (command.startsWith("SECURITY:"))
  {
    Firebase.RTDB.setInt(&fbdo, "/SecuritySystem/Alarm", (command.substring(9).toInt()));
  }
}
