#ifndef __InternetConnection_H
#define __InternetConnection_H

#include <ArduinoOTA.h>
#include <ESP8266WiFi.h>
#include <ThingSpeak.h>
#include <MetheoData.h>
#include <Thermostat.h>
#include <EEPROM.h>

class InternetConnection
{
public:
  bool initializeThingSpeak(void);
  bool initializeBlynk(void);
  void setMeteoDataToThingSpeakObject(MetheoData);
  bool sendDataToThingSpeakApi(void);
  void runBlynk(void);
  void initializeOTA(void);
  void handleOTA(void);
  bool sendDataToBlynk(MetheoData);
  static void setStatusToBlynk(String, String);
  static void setIsHeatingToBlynk(bool);
  static void callThermostatControll(void);
  static void processThermostatStatus(ThermostatStatus);
};

#endif