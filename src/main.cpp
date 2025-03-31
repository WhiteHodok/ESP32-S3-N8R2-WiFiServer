#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>

const char* ssid = "ESP32_Mikhaylyuck";
const char* password = "password";

WebServer server(80);

// HTML страница с формой ввода
const char* htmlPage = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32 Message Sender</title>
  <style>
    body { 
      font-family: Arial, sans-serif; 
      text-align: center; 
      margin-top: 50px; 
    }
    input, button { 
      padding: 10px; 
      margin: 5px; 
    }
    #message { 
      width: 300px; 
    }
    #status {
      color: green;
      margin-top: 20px;
    }
  </style>
</head>
<body>
  <h1>ESP32 Message Sender</h1>
  <form action="/send" method="POST">
    <input type="text" id="message" name="message" placeholder="Type your message here" required>
    <button type="submit">Send</button>
  </form>
  <div id="status">%STATUS%</div>
</body>
</html>
)rawliteral";

void handleRoot() {
  String page = htmlPage;
  page.replace("%STATUS%", "");
  server.send(200, "text/html", page);
}

void handleSend() {
  if (server.hasArg("message")) {
    String message = server.arg("message");
    Serial.println("Received message: " + message);
    
    String page = htmlPage;
    page.replace("%STATUS%", "Message sent successfully!");
    server.send(200, "text/html", page);
  } else {
    server.send(400, "text/plain", "Bad Request: No message found");
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}

void WiFiEvent(WiFiEvent_t event, WiFiEventInfo_t info) {
  switch(event) {
    case ARDUINO_EVENT_WIFI_AP_STACONNECTED:
      Serial.println("New device connected");
      Serial.print("MAC: ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", info.wifi_ap_staconnected.mac[i]);
        if (i < 5) Serial.print(":");
      }
      Serial.println();
      Serial.println("Current connected devices: " + String(WiFi.softAPgetStationNum()));
      break;
      
    case ARDUINO_EVENT_WIFI_AP_STADISCONNECTED:
      Serial.println("Device disconnected");
      Serial.print("MAC: ");
      for (int i = 0; i < 6; i++) {
        Serial.printf("%02X", info.wifi_ap_stadisconnected.mac[i]);
        if (i < 5) Serial.print(":");
      }
      Serial.println();
      Serial.println("Current connected devices: " + String(WiFi.softAPgetStationNum()));
      break;
      
    default:
      break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("Setting up AP...");
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Регистрируем обработчик WiFi событий
  WiFi.onEvent(WiFiEvent);

  server.on("/", handleRoot);
  server.on("/send", HTTP_POST, handleSend);
  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}
