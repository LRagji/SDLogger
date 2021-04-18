#include <DNSServer.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

const char* host = "VLogger";//Can be moved to FLASH
ESP8266WebServer server(80);
DNSServer dnsServer;
const byte DNS_PORT = 53;

void handleRoot() {
  server.send(200, "text/html", "<h1>You are connected</h1>");
}

void setup() {
  delay(1000);//For ESP to settle down
  WiFi.mode(WIFI_AP);
  WiFi.softAP(host);
  char dnsHostName [12 + 1];
  strcpy (dnsHostName, "VLogger");
  dnsServer.start(DNS_PORT, "*", WiFi.softAPIP());
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
