#include <DHT.h>
#include <WiFi.h>
#include <DHT_U.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>
#include <Firebase_ESP_Client.h>

#define DHTPIN 4      // Pin connected to the DHT11 sensor
#define DHTTYPE DHT11 // Define the type of DHT sensor

/* Wi-Fi and Firebase credentials */
#define WIFI_SSID "Ali Hubail"                                              // Replace with your Wi-Fi SSID
#define WIFI_PASSWORD "Alooy1123"                                             // Replace with your Wi-Fi Password
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

// Stepper Motor pins and configuration for garage system
#define IN1 4
#define IN2 5
#define IN3 22
#define IN4 23
AccelStepper garageMotor(8, IN1, IN3, IN2, IN4); // Use 8 for HALFSTEP mode
bool garageOpen = false;                         // Garage open/close state

DHT dht(DHTPIN, DHTTYPE);        // Initialize the DHT sensor
bool environmentAutoMode = true; // Automatic environment control mode
const int tempThreshold = 30;    // Temperature threshold for AC in °C
bool acBOOL = false;             // Manual AC control state
const int acGreenLedPin = 14;    // Green LED for AC ON
const int acRedLedPin = 12;      // Red LED for AC OFF

// Garbage Basket System pins
const int trigPin = 18;          // Ultrasonic sensor trigger pin
const int echoPin = 19;          // Ultrasonic sensor echo pin
const int servoPin = 21;         // Servo motor pin for basket
const int garbageThreshold = 20; // Distance threshold to open the basket (in cm)
// Servo object
Servo garbageServo;

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
const int soundThreshold = 20; // Threshold for sound sensor (adjust as needed)

int ArmedMode = 0;

const int moistureThreshold = 3000;
bool WaterPump = false;
bool gardenAutoMode = true;

void setup()
{
  Serial.begin(115200);                          // Debugging
  ArduinoSerial.begin(9600, SERIAL_8N1, 16, 17); // Communication with Arduino

  // Garage system setup
  garageMotor.setMaxSpeed(1000);    // Maximum speed in steps per second
  garageMotor.setAcceleration(500); // Acceleration in steps per second^2

  dht.begin(); // Initialize DHT11 sensor
  pinMode(acGreenLedPin, OUTPUT);
  pinMode(acRedLedPin, OUTPUT);

  // Initialize garbage basket pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  garbageServo.attach(servoPin);
  garbageServo.write(0); // Close the basket initially

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

  handleGarbageBasketSystem();

  handleGarageDoor();

  // Handle communication with Arduino
  handleArduinoCommunication();
}

int getBasketDistance()
{
  long duration;
  int distance;

  // Trigger the ultrasonic sensor
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure distance
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2; // Convert to cm
  Serial.println("The Des: " + String(distance));
  return distance;
}

void handleGarbageBasketSystem()
{
  int distance = getBasketDistance();

  if (distance > 0 && distance < garbageThreshold)
  {
    delay(1000);
    garbageServo.write(90); // Open the basket
    Serial.println("Basket Opened.");
  }
  else
  {
    garbageServo.write(0); // Close the basket
    Serial.println("Basket Closed.");
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

  // Security System mode
  Firebase.RTDB.getBool(&fbdo, "/SecuritySystem/Armed", &ArmedMode);
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
    garageMotor.moveTo(10240); // Move forward (open position) 1rev = 2048
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
    // Read incoming command from Arduino
    String command = ArduinoSerial.readStringUntil('\n');
    command.trim();
    processArduinoCommand(command); // Process the command
  }
}

// Function to process commands from Arduino
void processArduinoCommand(String command)
{
  if (command.startsWith("SECURITY:"))
  {
    Firebase.RTDB.setInt(&fbdo, "/SecuritySystem/Alarm", (command.substring(9).toInt()));
  }
  else if (command.startsWith("SECURITY_ARMED:"))
  {
    Firebase.RTDB.setInt(&fbdo, "/SecuritySystem/Armed", (command.substring(15).toInt()));
  }
}
