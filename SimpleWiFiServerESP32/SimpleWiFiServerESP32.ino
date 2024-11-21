#include <WiFi.h>

const char *ssid = "wifi";
const char *password = "password";

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(2, OUTPUT);  // LED pin

  delay(10);

  // Connect to WiFi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) {
    Serial.println("New Client.");
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
            client.println("<html lang=\"en\">");
            client.println("<head>");
            client.println("<meta charset=\"UTF-8\">");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
            client.println("<title>ESP32 LED Control Panel</title>");
            client.println("<style>");
            client.println("body { font-family: Arial, sans-serif; text-align: center; margin: 0; padding: 20px; background-color: #f4f4f4; }");
            client.println("h1 { color: #333; }");
            client.println("button { padding: 10px 20px; margin: 10px; background-color: #007bff; color: white; border: none; border-radius: 5px; cursor: pointer; }");
            client.println("button:hover { background-color: #0056b3; }");
            client.println("#log-area { width: 90%; max-width: 600px; height: 200px; margin: 20px auto; padding: 10px; border: 1px solid #ccc; background: #fff; overflow-y: scroll; }");
            client.println("</style>");
            client.println("<script>");
            client.println("function sendCommand(command) {");
            client.println("  fetch(command).then(response => {");
            client.println("    if (response.ok) {");
            client.println("      appendLog(`Command sent: ${command}`);");
            client.println("    } else {");
            client.println("      appendLog(`Failed to send command: ${command}`);");
            client.println("    }");
            client.println("  }).catch(error => appendLog(`Error: ${error}`));");
            client.println("}");
            client.println("function appendLog(message) {");
            client.println("  const logArea = document.getElementById('log-area');");
            client.println("  logArea.innerHTML += message + '<br>'; logArea.scrollTop = logArea.scrollHeight;");
            client.println("}");
            client.println("function clearLogArea() { document.getElementById('log-area').innerHTML = ''; }");
            client.println("</script>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>Smart Home Controller</h1>");
            client.println("<h2>Control Panel</h2>");
            client.println("<button onclick=\"sendCommand('/on')\">Turn LED On</button>");
            client.println("<button onclick=\"sendCommand('/off')\">Turn LED Off</button>");
            client.println("<h2>Logs</h2>");
            client.println("<div id=\"log-area\"></div>");
            client.println("<br>");
            client.println("<button onclick=\"clearLogArea()\">Clear Logs</button>");
            client.println("</body>");
            client.println("</html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // Handle commands
        if (currentLine.endsWith("GET /on")) {
          digitalWrite(2, HIGH); // Turn LED on
        } else if (currentLine.endsWith("GET /off")) {
          digitalWrite(2, LOW); // Turn LED off
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
