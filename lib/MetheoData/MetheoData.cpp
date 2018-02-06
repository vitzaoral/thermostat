#include "MetheoData.h"

SHT3X sht30(0x44);
Adafruit_BMP085 bmp;

// Initialize and get metheorological data
MetheoData::MetheoData()
{
    if (!bmp.begin())
    {
        Serial.println("Could not find a valid BMP100 sensor on oaddress 0x45!");
    }
}

void MetheoData::setData(void)
{
    shtTemperature = 0;
    shtHumidity = 0;
    bmpPresure = bmp.readPressure() / 100.0;
    averageTemperature = bmpTemperature = bmp.readTemperature();

    if (sht30.get() == 0)
    {
        shtTemperature = sht30.cTemp;
        shtHumidity = sht30.humidity;
        averageTemperature = (bmpTemperature + shtTemperature) / 2.0;
    }
    else
    {
        Serial.println("SHT30 sensor error!");
    }
}

bool MetheoData::dataAreValid(void)
{
    return shtTemperature <= 50.0 && shtTemperature >= 5.0 &&
           shtHumidity <= 100.0 && shtHumidity >= 0.0 &&
           bmpPresure <= 1200.0 && bmpPresure >= 800.0 &&
           bmpTemperature <= 50.0 && bmpTemperature >= 5.0 &&
           averageTemperature <= 50.0 && averageTemperature >= 5.0;
}
