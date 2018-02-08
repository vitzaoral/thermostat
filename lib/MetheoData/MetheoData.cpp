#include "MetheoData.h"

Adafruit_SHT31 sht31 = Adafruit_SHT31();

// Initialize and get metheorological data
MetheoData::MetheoData()
{
    if (!sht31.begin(0x44))
    {
        Serial.println("Could not find a valid SHT31X sensor on oaddress 0x44!");
    }
}

void MetheoData::setData(void)
{
    shtTemperature = sht31.readTemperature();
    shtHumidity = sht31.readHumidity();

    if (isnan(shtTemperature))
    {
        Serial.println("Error during read temperature");
    }
    if (isnan(shtHumidity))
    {
        Serial.println("Error during read humidity");
    }

    Serial.println(shtTemperature);
    Serial.println(shtHumidity);
}

bool MetheoData::dataAreValid(void)
{
    return shtTemperature <= 50.0 && shtTemperature >= 5.0 &&
           shtHumidity <= 100.0 && shtHumidity >= 0.0;
}
