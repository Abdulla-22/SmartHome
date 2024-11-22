#include <WiFi.h>
#include <HardwareSerial.h>

// Define a custom UART instance
HardwareSerial MySerial(2);
String TEXT = "";

const char *ssid = "Zain_H122_CB1E";
const char *password = "M6GJ2QEF53J";

WiFiServer server(80);

void setup() {
  Serial.begin(115200); // Debugging Serial Monitor
  MySerial.begin(9600, SERIAL_8N1, 16, 17); // GPIO16 as RX, GPIO17 as TX

  pinMode(2, OUTPUT); // ESP32 built-in LED pin
  digitalWrite(2, LOW); // Turn off LED initially

  // Connect to WiFi
  Serial.println("\nConnecting to WiFi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  TestReciveData();

  Serial.println("\nWiFi connected.");
  Serial.println("IP address: " + WiFi.localIP().toString());

  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) {
    Serial.println("New Client connected.");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Serve the HTML page
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

              // HTML content
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head>");
            client.println("<title>ESP32 LED Control</title>");
            client.println("<style>");
            client.println("body {");
            client.println("  display: flex;");
            client.println("  flex-direction: column;");
            client.println("  align-items: center;");
            client.println("  justify-content: center;");
            client.println("  min-height: 100vh;");
            client.println("  margin: 0;");
            client.println("  font-family: 'Arial', sans-serif;");
            client.println("  background: linear-gradient(135deg, #74ebd5, #ACB6E5);");
            client.println("  color: #333;");
            client.println("}");
            client.println("h1 {");
            client.println("  margin-bottom: 20px;");
            client.println("  font-size: 2.5rem;");
            client.println("  color: #fff;");
            client.println("}");
            client.println("button {");
            client.println("  padding: 15px 30px;");
            client.println("  margin: 10px;");
            client.println("  font-size: 1.2rem;");
            client.println("  font-weight: bold;");
            client.println("  color: #fff;");
            client.println("  background: #4CAF50;");
            client.println("  border: none;");
            client.println("  border-radius: 8px;");
            client.println("  cursor: pointer;");
            client.println("  box-shadow: 0 4px 6px rgba(0, 0, 0, 0.2);");
            client.println("  transition: background 0.3s ease, transform 0.2s;");
            client.println("}");
            client.println("button:hover {");
            client.println("  background: #45a049;");
            client.println("  transform: translateY(-2px);");
            client.println("}");
            client.println("button:active {");
            client.println("  transform: translateY(0);");
            client.println("  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.2);");
            client.println("}");
            client.println("footer {");
            client.println("  margin-top: 20px;");
            client.println("  font-size: 0.9rem;");
            client.println("  color: #eee;");
            client.println("}");
            client.println("</style>");
            client.println("<script>");
            client.println("function sendCommand(command) {");
            client.println("  fetch(command).then(response => {");
            client.println("    if (response.ok) {");
            client.println("    } else {");
            client.println("      alert('Failed to send command');");
            client.println("    }");
            client.println("  }).catch(error => alert('Error: ' + error));");
            client.println("}");
            client.println("</script>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>ESP32 LED Control</h1>");
            client.println("<button onclick=\"sendCommand('/on')\">Turn ON</button>");
            client.println("<button onclick=\"sendCommand('/off')\">Turn OFF</button>");
            client.println("<footer>ESP32 LED Controller © 2024</footer>");
            client.println("</body>");
            client.println("</html>");

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // Handle GET requests for /on and /off
        if (currentLine.endsWith("GET /on")) {
          Serial.println("Command received: LED_ON");
          MySerial.println("LED_ON"); // Send command to Arduino

          TEXT = MySerial.readStringUntil('\n'); // Read the incoming command
          TEXT.trim(); // Remove any extra whitespace

          Serial.print("The received command: ");
          Serial.println(TEXT);

          TestReciveData();
        } else if (currentLine.endsWith("GET /off")) {
          Serial.println("Command received: LED_OFF");
          MySerial.println("LED_OFF"); // Send command to Arduino
          TestReciveData();
        }
      }
    }

    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
  }
}

// if the data has been reseved the blue LED will blink
void TestReciveData() {
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);
  delay(100);
  digitalWrite(2, HIGH);
  delay(100);
  digitalWrite(2, LOW);

}
