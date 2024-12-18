#include <WiFi.h>
#include <WebServer.h>

// Wi-Fi credentials
const char* ssid = "SHIN";
const char* password = "shin0412";

// Web Server configuration
WebServer server(80);

// UART1 configuration
#define RXD1 16
#define TXD1 17

// Buffer to store UART data
#define BUFFER_SIZE 4096
char dataBuffer[BUFFER_SIZE] = "Waiting for data from STM32...";
uint16_t bufferIndex = 0;

// Function to handle root page
void handleRoot() {
  String html = "<html><head><title>ESP32 Web Server</title>";
  html += "<meta http-equiv='refresh' content='2'></head><body>";
  html += "<h1>ESP32 Web Server</h1>";
  html += "<p>Latest UART Data:</p><pre>";
  html += dataBuffer;
  html += "</pre></body></html>";
  server.send(200, "text/html", html);
}

// Function to handle /data page
void handleDataRequest() {
  Serial.println("Handling /data request...");
  server.send(200, "text/plain", dataBuffer); // Send buffer content to client
  Serial.println("Data sent to client.");
}

void setup() {
  Serial.begin(115200);  // Debug output
  Serial.println("Starting ESP32 and UART1...");

  // Initialize UART1
  Serial1.begin(115200, SERIAL_8N1, RXD1, TXD1);
  Serial.println("UART1 initialized!");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to Wi-Fi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi connected!");
  Serial.print("ESP32 IP Address: ");
  Serial.println(WiFi.localIP());

  // Register endpoints
  server.on("/", HTTP_GET, handleRoot);         // Root page
  server.on("/data", HTTP_GET, handleDataRequest); // Endpoint for UART data

  // Start Web Server
  server.begin();
  Serial.println("Web Server is running!");
}

void loop() {
  server.handleClient();  // Handle web client requests

  // Continuously read UART data

  while (Serial1.available()) {
    char c = Serial1.read();

    // If we reach the end marker '!' or buffer is full
    if (c == '!' || bufferIndex >= BUFFER_SIZE - 1) {
      dataBuffer[bufferIndex] = '\0'; // End the string
      Serial.print("Data received from UART: ");
      Serial.println(dataBuffer);

      bufferIndex = 0;  // Reset buffer index to start overwriting
      break; // Exit the loop
    } else {
      dataBuffer[bufferIndex++] = c; // Add character to buffer
    }
      //Serial.println(WiFi.localIP());
   // delay(500);
  }
}