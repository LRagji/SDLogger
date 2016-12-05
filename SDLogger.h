/*
  SDLogger.h - Library for logging Warning, Error and Info to SD card and Serial console.
  Created by Laukik Ragji, December 06, 2016.
  Released with MIT.
*/

#ifndef SDLogger_h
#define SDLogger_h

#include "Arduino.h"
#include <SD.h>
#include <SPI.h>

class SDLogger
{
public:
  SDLogger();
  void Begin(byte maxTries, long timeDelayInMSPerRetry, String filePath);
  void LogInfo(String header , String info);
  void LogWarn(String header , String info);
  void LogError(String header , String info);
private:
 String logFilePath;
 void WriteToSD(String data);
};

#endif
