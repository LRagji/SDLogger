/*
  SDLogger.cpp - Library for logging Warning, Error and Info to SD card and Serial console.
  Created by Laukik Ragji, December 06, 2016.
  Released with MIT.
*/

#include "Arduino.h"
#include <SD.h>
#include <SPI.h>
#include "SDLogger.h"

SDLogger::SDLogger()
{
}

void SDLogger::Begin(byte maxTries, long timeDelayInMSPerRetry, String filePath)
{
  byte retries = 0;
  while (!Serial & retries < maxTries)
  {
    delay(timeDelayInMSPerRetry);
    retries++;
  }
  logFilePath = filePath;
}

void SDLogger::LogInfo(String header , String info)
{
  if (Serial)
  {
    Serial.println("INFO:" + header  + info);
  }
  
  WriteToSD("INFO," + header + "," + info);
}

void SDLogger::LogWarn(String header , String info)
{
  if (Serial)
  {
    Serial.println("WARN:" + header  + info);
  }
  
  WriteToSD("WARN," + header + "," + info);
}

void SDLogger::LogError(String header , String info)
{
  if (Serial)
  {
    Serial.println("ERR:" + header  + info);
  }
  
  WriteToSD("ERR," + header + "," + info);
}

void SDLogger::WriteToSD(String data)
{
  File logFile = SD.open(logFilePath, FILE_WRITE);
  if (logFile)
  {
    logFile.println(data);
    logFile.close();
  }
}