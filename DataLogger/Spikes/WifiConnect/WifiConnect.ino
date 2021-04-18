#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ESP8266mDNS.h>
#include <EEPROM.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include<ADS1115_WE.h>
#include<Wire.h>
#include "SPI.h"
#include "SD.h"

// DNS server
DNSServer dnsServer;

// Web server
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

//UDP for NTP
WiFiUDP ntpUDP;

//ADC
ADS1115_WE adc(0x48);

/* Don't set this wifi credentials. They are configurated at runtime and stored on EEPROM */
char ssid[33] = "";
char password[65] = "";
char runMode = 'S';
const char *myHostname = "logger";
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000);
ADS1115_MUX adcChannels[] = {ADS1115_COMP_0_GND, ADS1115_COMP_1_GND, ADS1115_COMP_2_GND, ADS1115_COMP_3_GND};
byte adcChannelLength = (sizeof(adcChannels) / sizeof(ADS1115_MUX));
byte adcNumberOfReadings = 4 ;
byte adcBuffRolloverPoint  = (adcChannelLength + 1) * ((adcNumberOfReadings * 2) + 1);
long adcCircularBuff[255];
byte adcCircularIndexer = 0;
byte adcSampleInterval = 5;
long adcLastRun = 0;


void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  if ( loadCredentials() == true && connectWifi() == WL_CONNECTED)
  {
    //Wifi Connected
    runMode = 'R';
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
    // Setup MDNS responder
    if (!MDNS.begin(myHostname)) {
      Serial.println("Error setting up MDNS responder!");
    } else {
      Serial.println("mDNS responder started");
      // Add service to MDNS-SD
      MDNS.addService("http", "tcp", 80);
    }
    Wire.begin();
    timeClient.begin();
    ws.onEvent(onEvent);
    server.addHandler(&ws);
    server.on("/reset", HTTP_GET, handleReset);
    server.onNotFound(handleNotFound);
    if (adcSetup() == false || initSDHttpServer() == false)
    {
      runMode = 'E';
    }
  }
  else
  {
    //Access point
    runMode = 'C';
    Serial.println("Configuring access point...");
    IPAddress apIP(172, 217, 28, 1);
    IPAddress netMsk(255, 255, 255, 0);
    WiFi.softAPConfig(apIP, apIP, netMsk);
    WiFi.softAP("Logger");
    delay(500); // Without delay I've seen the IP address blank
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
    const byte DNS_PORT = 53;
    dnsServer.start(DNS_PORT, "*", apIP);
    //server.on("/", HTTP_GET, handleRoot);
    server.on("/", HTTP_GET, handleWifi);
    server.on("/wifisave", HTTP_POST, handleWifiSave);
    server.onNotFound(handleWifi);
  }

  server.begin(); // Web server start
  Serial.println("HTTP server started");
}

unsigned int connectWifi() {
  Serial.println("Connecting as wifi client...");
  WiFi.disconnect();
  WiFi.begin(ssid, password);
  unsigned int connRes = WiFi.waitForConnectResult();
  Serial.print("connRes: ");
  Serial.println(connRes);
  return connRes;
}

void loop() {

  switch (runMode)
  {
    case 'R'://Normal mode
      timeClient.update();
      MDNS.update();
      ws.cleanupClients();
      adcReadFillValues();
      break;
    case 'C': //Config mode
      dnsServer.processNextRequest();
      break;
    default:
      Serial.print("Unknown Mode:");
      Serial.println(runMode);
      delay(1000);
  }
  //HTTP
  //server.handleClient();
}
