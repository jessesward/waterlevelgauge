
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include "SPIFFS.h"

#include "NormalizeBetween.h"
#include "OLED_Display.h"
#include "config.h"

user::OLED_Display display;            // Create an instance of the OLED_Display class
AsyncWebServer server(80);             // Create AsyncWebServer object on port 80
AsyncWebSocket ws("/ws");              // Create a WebSocket object
IPAddress local_IP(192, 168, 86, 111); // Set your Static IP address
IPAddress gateway(192, 168, 86, 1);    // Set your Gateway IP address
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);   // optional
IPAddress secondaryDNS(8, 8, 4, 4); // optional

#define TRIG_PIN 32
#define ECHO_PIN 34
#define DISPLAY_SDA 26
#define DISPLAY_SLC 27
#define MAX_WATER 160
#define MIN_WATER 926

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 1000;

// Initialize SPIFFS
void initSPIFFS()
{
  if (!SPIFFS.begin(true))
  {
    Serial.println("An error has occurred while mounting SPIFFS");
  }
  Serial.println("SPIFFS mounted successfully");
}

void onWifiConnected(WiFiEvent_t wifi_event, WiFiEventInfo_t wifi_info)
{
  Serial.println(WiFi.localIP());
}

void initWiFi()
{
  // Configures static IP address
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS))
  {
    Serial.println("STA Failed to configure");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi ...");
  WiFi.onEvent(onWifiConnected, ARDUINO_EVENT_WIFI_STA_GOT_IP);
}

void notifyClients(String sensorReadings)
{
  ws.textAll(sensorReadings);
}

void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
  {
  }
}

void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
  switch (type)
  {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

void initWebSocket()
{
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

void setup()
{
  Serial.begin(115200);
  initWiFi();
  initSPIFFS();
  initWebSocket();
  display.init(DISPLAY_SDA, DISPLAY_SLC);

  pinMode(TRIG_PIN, OUTPUT); // Sets the trigPin as an Output
  pinMode(ECHO_PIN, INPUT);  // Sets the echoPin as an Input

  // Web Server Root URL
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
            { request->send(SPIFFS, "/index.html", "text/html"); });

  server.serveStatic("/", SPIFFS, "/");

  // Start server
  server.begin();
}

int calculateDistance()
{
  // Clears the trigPin
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  // Reads the echoPin, returns the sound wave travel time in microseconds
  long duration = pulseIn(ECHO_PIN, HIGH);
  // Calculating the distance
  // int distanceCM = duration * 0.034 / 2;
  return duration;
}
void loop()
{

  if ((millis() - lastTime) > timerDelay)
  {
    int distance = calculateDistance();
    double tankPercent = normalizeBetween(distance, MAX_WATER, MIN_WATER, 100, 0);
    double tankMin = (tankPercent < 100) ? tankPercent : 100;
    String percentUnits = String(tankMin) + "%";
    Serial.println(distance);
    display.write(percentUnits);
    String data = "{ \"distance\": \"" + String(tankMin) + "\"}";

    notifyClients(data);
    lastTime = millis();
  }

  ws.cleanupClients();

  delay(100);
}
