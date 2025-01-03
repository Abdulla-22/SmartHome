#include <DHT.h>
#include <WiFi.h>
#include <DHT_U.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>
#include <Firebase_ESP_Client.h>

// DHT sensor configuration
#define DHTPIN 4
#define DHTTYPE DHT11

// Wi-Fi and Firebase credentials
#define WIFI_SSID "Ali Hubail"
#define WIFI_PASSWORD "Alooy1123"
#define API_KEY "AIzaSyB0S9NtzkxVAHmU3A-7zLnE3-yTbrPB4dY"
#define DATABASE_URL "https://espcontroller-dc45b-default-rtdb.firebaseio.com/"
#define USER_EMAIL "abdulla38898@gmail.com"
#define USER_PASSWORD "12345678"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;

// Serial communication with Arduino
HardwareSerial ArduinoSerial(2); // UART2

// Stepper Motor configuration for garage system
#define IN1 5
#define IN2 13
#define IN3 22
#define IN4 23
AccelStepper garageMotor(8, IN1, IN3, IN2, IN4);
bool garageOpen = false;

// DHT sensor and environment control
DHT dht(DHTPIN, DHTTYPE);
bool environmentAutoMode = true;
const int tempThreshold = 30;
bool acBOOL = false;
const int acGreenLedPin = 14;
const int acRedLedPin = 12;

// Garbage Basket System
const int trigPin = 18;
const int echoPin = 19;
const int servoPin = 21;
const int garbageThreshold = 10;
Servo garbageServo;

// Lighting System
const int ldrPin = 34;
const int outdoorLedPin = 25;
const int indoorLedPin = 26;
const int soundSensorPin = 35;
bool outdoorLightAuto = true;
bool indoorLightAuto = true;
int outdoorLightState = false;
int indoorLightState = false;
const int ldrThreshold = 500;
const int soundThreshold = 50;

// Garden System
const int waterPumpPin = 27;
const int soilMoisturePin = 32;
const int rainSensorPin = 33;
const int moistureThreshold = 3000;
bool gardenAutoMode = true;
bool WaterPump = false;

// Security System
int ArmedMode = 0;

// Variables for garbage basket system
int lastState = 0; // 0 = closed, 1 = open

void setup() {
  Serial.begin(115200);
  ArduinoSerial.begin(9600, SERIAL_8N1, 16, 17);

  // Initialize sensors and actuators
  dht.begin();
  pinMode(acGreenLedPin, OUTPUT);
  pinMode(acRedLedPin, OUTPUT);
  pinMode(outdoorLedPin, OUTPUT);
  pinMode(indoorLedPin, OUTPUT);
  pinMode(waterPumpPin, OUTPUT);
  pinMode(soilMoisturePin, INPUT);
  pinMode(rainSensorPin, INPUT);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  garbageServo.attach(servoPin);
  garbageServo.write(0); // Close the basket initially

  // Garage motor setup
  garageMotor.setMaxSpeed(1000);
  garageMotor.setAcceleration(500);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
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

void loop() {
  // Periodic tasks
  readDB();
  handleLightingSystem();
  handleGardenSystem();
  handleEnvironmentControl();
  handleGarbageBasketSystem();
  handleGarageDoor();
  handleArduinoCommunication();
  handleSecuritySystem(); // Check security system status
}

// Garbage basket distance measurement
int getBasketDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2;
  Serial.println("Basket Distance: " + String(distance));
  return distance;
}

// Move servo motor gradually
void moveServoSlowly(Servo &servo, int startAngle, int endAngle, int delayMs) {
  if (startAngle < endAngle) {
    for (int angle = startAngle; angle <= endAngle; angle++) {
      servo.write(angle);
      delay(delayMs);
    }
  } else {
    for (int angle = startAngle; angle >= endAngle; angle--) {
      servo.write(angle);
      delay(delayMs);
    }
  }
}

// Handle garbage basket system
void handleGarbageBasketSystem() {
  int distance = getBasketDistance();
  if (distance > 0 && distance < garbageThreshold && lastState == 0) {
    delay(1000);
    moveServoSlowly(garbageServo, 0, 50, 20);
    Serial.println("Basket Opened.");
    lastState = 1;
  } else if (distance > garbageThreshold + 5 && lastState == 1) {
    moveServoSlowly(garbageServo, 50, 0, 20);
    Serial.println("Basket Closed.");
    lastState = 0;
  }
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
  // Read various system states from Firebase
  Firebase.RTDB.getBool(&fbdo, "/LightingSystem/Outdoor/Auto", &outdoorLightAuto);
  Firebase.RTDB.getBool(&fbdo, "/LightingSystem/Indoor/Auto", &indoorLightAuto);
  Firebase.RTDB.getInt(&fbdo, "/LightingSystem/Outdoor/State", &outdoorLightState);
  Firebase.RTDB.getInt(&fbdo, "/LightingSystem/Indoor/State", &indoorLightState);

  Firebase.RTDB.getBool(&fbdo, "/GardenSystem/WaterPump", &WaterPump);
  Firebase.RTDB.getBool(&fbdo, "/GardenSystem/Auto", &gardenAutoMode);

  Firebase.RTDB.getBool(&fbdo, "/EnvironmentSystem/Auto", &environmentAutoMode);
  Firebase.RTDB.getBool(&fbdo, "/EnvironmentSystem/ManualAC", &acBOOL);
  // Read the garage door open/close status
  Firebase.RTDB.getBool(&fbdo, "/EnvironmentSystem/GarageSystemStute", &garageOpen);

  // Security System mode (armed/disarmed)
  Firebase.RTDB.getBool(&fbdo, "/SecuritySystem/Armed", &ArmedMode); // Read Armed mode
  Serial.println("Security Mode: " + String(ArmedMode ? "Armed" : "Disarmed"));
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
    // Firebase.RTDB.getBool(&fbdo, "/GardenSystem/WaterPump", &WaterPump);

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
      if (!Firebase.RTDB.setBool(&fbdo, "/EnvironmentSystem/ACState", true))
      {
        Serial.println("Failed to log AC State: " + fbdo.errorReason());
      }
    }
    else
    {
      digitalWrite(acGreenLedPin, LOW); // Turn off the green LED (AC OFF)
      digitalWrite(acRedLedPin, HIGH);  // Turn on the red LED (AC OFF)
      Serial.println("Air Conditioner OFF.");
      if (!Firebase.RTDB.setBool(&fbdo, "/EnvironmentSystem/ACState", false))
      {
        Serial.println("Failed to log AC State: " + fbdo.errorReason());
      }
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

  // // Log AC state to Firebase
  // if (!Firebase.RTDB.setBool(&fbdo, "/EnvironmentSystem/ACState", digitalRead(acGreenLedPin) == HIGH))
  // {
  //   Serial.println("Failed to log AC State: " + fbdo.errorReason());
  // }
}

void handleGarageDoor()
{
  if (garageOpen)
  {
    Serial.println("Opening Garage Door...");
    garageMotor.moveTo(1100); // Move forward (open position) 1rev = 2048
    while (garageMotor.distanceToGo() != 0)
    {
      garageMotor.run();
    }
    delay(500); // Stabilize after reaching position
  }
  else
  {
    Serial.println("Closing Garage Door...");
    garageMotor.moveTo(0); // Move backward (closed position)
    while (garageMotor.distanceToGo() != 0)
    {
      garageMotor.run();
    }
    delay(500); // Stabilize after reaching position
  }
}

// Function to handle communication with Arduino
void handleArduinoCommunication()
{
  if (ArduinoSerial.available())
  {
    // Read incoming command from ESP32
    String command = ArduinoSerial.readStringUntil('\n');
    command.trim();
    processArduinoCommand(command); // Process the command
  }
}

// Function to process commands from Arduino
void processArduinoCommand(String command)
{
  if (command.startsWith("SECURITY_ARMED:"))
  {
    int armedStatus = command.substring(15).toInt();
    if (armedStatus == 1)
    {
      Serial.println("Security system is ARMED");
      Firebase.RTDB.setBool(&fbdo, "/SecuritySystem/Armed", true);

      // Handle armed mode actions (e.g., activate alarm, lock doors)
    }
    else
    {
      Serial.println("Security system is DISARMED");
      Firebase.RTDB.setBool(&fbdo, "/SecuritySystem/Armed", false);

      // Handle disarmed mode actions (e.g., deactivate alarm, unlock doors)
    }
  }
}

void handleSecuritySystem()
{
  // Check if the security system is armed or disarmed
  if (ArmedMode == 1)
  {
    // Perform actions when armed (e.g., trigger alarm, lock doors, etc.)
    Serial.println("Security system is ARMED");
    ArduinoSerial.println("SECURITY_MODE:1"); // Send armed status to Arduino
  }
  else
  {
    // Perform actions when disarmed (e.g., deactivate alarm, unlock doors, etc.)
    Serial.println("Security system is DISARMED");
    ArduinoSerial.println("SECURITY_MODE:0"); // Send disarmed status to Arduino
  }
}
