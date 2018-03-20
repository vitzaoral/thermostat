#include "Thermostat.h"

// PINS
const int relayPinAddress = D5;
const int redLed = D6;
const int greenLed = D7;
const int blueLed = D8;

const int heatingTimeInSeconds = 60;
int timer = 0;

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
                // if heating is OFF
                if (digitalRead(relayPinAddress) == LOW)
                {
                    setLed(greenLed, true);
                    digitalWrite(relayPinAddress, HIGH);
                    status = {(char *)"Heating ON", (char *)"#00FF00", true};

                    // start heating, set timer to 0 seconds
                    timer = 0;
                }
                else
                {
                    // TODO do nejake constanty tech 10 sekund
                    // TODO dodat nejaky max count po ktery muze topit, treba 30min apod.
                    // heating is ON, add 10 seconds according to readMetheoDataDisplayDataControllThermostatInterval in main.cpp
                    timer += 10;
                    status = {(char *)"Heating ON", (char *)"#00FF00", true};
                }
            }
            else
            {
                // if heating is ON, should turn OFF
                if (digitalRead(relayPinAddress) == HIGH)
                {
                    // look to timer, turn off after minute
                    if (timer >= heatingTimeInSeconds)
                    {
                        setLed(blueLed, true);
                        digitalWrite(relayPinAddress, LOW);
                        status = {(char *)"Heating OFF", (char *)"#FF0000", false};
                        timer = 0;
                    }
                    else
                    {
                        // count to 1 minute
                        timer += 10;
                        status = {(char *)"Heating (waiting to OFF)", (char *)"#00FF00", true};
                    }
                }
                else
                {
                    // heating was OFF
                    setLed(blueLed, true);
                    digitalWrite(relayPinAddress, LOW);
                    status = {(char *)"Heating OFF", (char *)"#FF0000", false};
                    timer = 0;
                }
            }
        }
        else
        {
            setLed(redLed, true);
            digitalWrite(relayPinAddress, LOW);
            status = {(char *)"Heating not enabled", (char *)"#FF0000", false};
            timer = 0;
        }
    }
    else
    {
        setLed(redLed, true);
        digitalWrite(relayPinAddress, LOW);
        status = {(char *)"Temperature data are invalid, heating OFF", (char *)"#FF0000", false};
        timer = 0;
    }
    return status;
}
