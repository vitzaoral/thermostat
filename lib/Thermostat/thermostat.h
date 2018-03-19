#ifndef __Thermostat_H
#define __Thermostat_H

#include <MetheoData.h>
#include <EEPROM.h>

struct ThermostatStatus
{
    char *message;
    char *color;
    bool isHeating;
};

class Thermostat
{
  public:
    static ThermostatStatus controllThermostat(MetheoData data);
    static void setLed(int ledPin, bool isTurnOn);
    static void turnOffLed();
    static void initialize();
};

#endif
