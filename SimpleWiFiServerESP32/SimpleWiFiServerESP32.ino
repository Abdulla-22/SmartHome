#include <WiFi.h>
#include <Firebase_ESP_Client.h>

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Zain_H122_CB1E"
#define WIFI_PASSWORD "M6GJ2QEF53J"

/* 2. Define the API Key */
#define API_KEY "AIzaSyB0S9NtzkxVAHmU3A-7zLnE3-yTbrPB4dY"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://espcontroller-dc45b-default-rtdb.firebaseio.com/" 

/* 4. Define the user Email and password that alreadey registerd or added in your project */
#define USER_EMAIL "abdulla38898@gmail.com"
#define USER_PASSWORD "12345678"

// Define Firebase Data object
FirebaseData fbdo;

FirebaseAuth auth;
FirebaseConfig config;

unsigned long sendDataPrevMillis = 0;

const int ledPin = 2;
HardwareSerial ArduinoSerial(2);

void setup()
{
  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, LOW);

  Serial.begin(115200);
  ArduinoSerial.begin(9600, SERIAL_8N1, 16, 17);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  /* Assign the api key (required) */
  config.api_key = API_KEY;

  /* Assign the user sign in credentials */
  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  /* Assign the RTDB URL (required) */
  config.database_url = DATABASE_URL;

  // Comment or pass false value when WiFi reconnection will control by your code or third party library e.g. WiFiManager
  Firebase.reconnectNetwork(true);

  // Since v4.4.x, BearSSL engine was used, the SSL buffer need to be set.
  // Large data transmission may require larger RX buffer, otherwise connection issue or data read time out can be occurred.
  fbdo.setBSSLBufferSize(4096 /* Rx buffer size in bytes from 512 - 16384 */, 1024 /* Tx buffer size in bytes from 512 - 16384 */);

  // Limit the size of response payload to be collected in FirebaseData
  fbdo.setResponseSize(2048);

  Firebase.begin(&config, &auth);

  Firebase.setDoubleDigits(5);

  config.timeout.serverResponse = 10 * 1000;
}

void loop()
{
  // Firebase.ready() should be called repeatedly to handle authentication tasks.
  if (Firebase.ready() && (millis() - sendDataPrevMillis > 1000 || sendDataPrevMillis == 0))
  {
    sendDataPrevMillis = millis();

  int ledState;
    // control the indor lighting
   if(Firebase.RTDB.getInt(&fbdo, "/LightingSystem/InDoor", &ledState)){
    digitalWrite(ledPin, ledState);
    if (ledState == 1) {
      ArduinoSerial.println("IN_DOOR_LED_ON");

    } else if (ledState == 0) {
      ArduinoSerial.println("IN_DOOR_LED_OFF");

    }
    
   }else{
    Serial.println(fbdo.errorReason().c_str());
   }
  }
}
