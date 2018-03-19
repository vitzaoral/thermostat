#include "Thermostat.h"

// PINS
const int relayPinAddress = D5;
const int redLed = D6;
const int greenLed = D7;
const int blueLed = D8;

void Thermostat::turnOffLed()
{
    // RGB LED is Anode type, so HIGH = turnOff, LOW = turnOn
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, HIGH);
    digitalWrite(blueLed, HIGH);
}

void Thermostat::initialize()
{
    // set pinmodes to LED and relay
    pinMode(redLed, OUTPUT);
    pinMode(greenLed, OUTPUT);
    pinMode(blueLed, OUTPUT);
    pinMode(relayPinAddress, OUTPUT);

    turnOffLed();
}

void Thermostat::setLed(int ledPin, bool isTurnOn)
{
    turnOffLed();
    digitalWrite(ledPin, isTurnOn ? LOW : HIGH);
}

// Set thermostat ON/OFF and temperature
ThermostatStatus Thermostat::controllThermostat(MetheoData data)
{
    ThermostatStatus status;
    if (data.dataAreValid())
    {
        // heating is enabled
        if (EEPROM.read(1) == true)
        {
            int requiredTemperature = EEPROM.read(2);
            if (requiredTemperature >= 10 && requiredTemperature <= 25 && data.shtTemperature <= requiredTemperature)
            {
                setLed(greenLed, true);
                digitalWrite(relayPinAddress, HIGH);
                status = { (char *)"Heating ON", (char *)"#00FF00", true };
            }
            else
            {
                setLed(blueLed, true);
                digitalWrite(relayPinAddress, LOW);
                status = { (char *)"Heating OFF", (char *)"#FF0000", false };
            }
        }
        else
        {
            setLed(redLed, true);
            digitalWrite(relayPinAddress, LOW);
            status = { (char *)"Heating not enabled", (char *)"#FF0000", false };
        }
    }
    else
    {
        setLed(redLed, true);
        digitalWrite(relayPinAddress, LOW);
        status = { (char *)"Data are invalid, heating OFF", (char *)"#FF0000", false };
    }
    return status;
}
