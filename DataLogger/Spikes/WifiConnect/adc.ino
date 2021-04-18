
bool adcSetup()
{
  if (!adc.init()) {
    Serial.println("ADS1115 not connected!");
    return false;
  }
  else
  {
    Serial.println("ADC Active");
  }
  adc.setVoltageRange_mV(ADS1115_RANGE_6144);
  adc.setAlertPinMode(ADS1115_ASSERT_AFTER_1);
  adc.setAlertPol(ADS1115_ACT_LOW);
  adc.setAlertPinToConversionReady();
  return true;
}

void adcReadFillValues()
{
  long timestamp = timeClient.getEpochTime();
  if (timestamp % adcSampleInterval == 0 && adcLastRun != timestamp)
  {
    adcLastRun = timestamp;
    adcCircularBuff[FAAIndexer()] = timestamp;
    for (byte indexer = 0; indexer < adcChannelLength; indexer++)
    {
      adcCircularBuff[FAAIndexer()] = readChannel(adcChannels[indexer]);
    }
  }
}


long readChannel(ADS1115_MUX channel) {
  long adcCount = 0;
  adc.setCompareChannels(channel);
  adc.startSingleMeasurement();
  while (adc.isBusy()) {}
  return  adc.getRawResult();
  //  Since adc can report negative and positive voltages and its 16 bit which means it can spit out adc count from −32,768 to 32,767.
  //  We are using long as return data type which means it can hold 32 bits (4 bytes), from -2,147,483,648 to 2,147,483,647
  //  To calculate total voltage we need to (adcCount/32768)* adcfactor Link here:https://github.com/wollewald/ADS1115_WE/blob/272d84079380e8be8f7ce5578bc308d0de549386/src/ADS1115_WE.cpp#L239
}

byte FAAIndexer()
{
  byte cache = adcCircularIndexer;
  if (cache == 0)
  {
    purgeBuffer();
  }
  adcCircularIndexer += 1;
  if (adcCircularIndexer >= adcBuffRolloverPoint)
  {
    adcCircularIndexer = 0;
  }
  return cache;
}

void purgeBuffer()
{
  String wsMsg = "";
  for (byte indexer = 0; indexer < adcBuffRolloverPoint; indexer++)
  {
    wsMsg += String(adcCircularBuff[indexer]);
    if ((indexer + 1) % (adcChannelLength + 1) == 0)
    {
      wsMsg += "\n";
    }
    else
    {
      wsMsg += ",";
    }
  }
  Serial.println(wsMsg);
  ws.textAll(wsMsg);
}
