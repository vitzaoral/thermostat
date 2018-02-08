#ifndef __MetheoData_H
#define __MetheoData_H

#include <Arduino.h>
#include <Adafruit_SHT31.h>

class MetheoData
{
public:
  MetheoData();
  float shtTemperature;
  float shtHumidity;

  void setData(void);
  bool dataAreValid(void);
};

#endif
