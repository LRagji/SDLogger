#include <SD.h>
#include <SPI.h>
#include <SDLogger.h>

const int chipSelect = 6;
SDLogger logger;

void setup() {

  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  }
  Serial.println("card initialized.");
  logger.Begin(3, 1000, "log.txt");
}

void loop() {

  logger.LogInfo("Pre", "InfoText");
  logger.LogWarn("Pre", "Laukik");
  logger.LogError("Pre", "olo");

  while (1);
}
