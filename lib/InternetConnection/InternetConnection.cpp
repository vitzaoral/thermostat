#include "InternetConnection.h"

#include "../../src/settings.cpp"
#include <BlynkSimpleEsp8266.h>

WiFiClient client;
Settings settings;

const char *thingSpeakWriteApiKey = settings.thingSpeakWriteApiKey;
const unsigned long thingSpeakChannelId = settings.thingSpeakChannelId;
const char *ssid = settings.ssid;
const char *password = settings.password;
const char *blynkAuth = settings.blynkAuth;

// number of attempts to connecting WIFI, API etc.
const int timeout = 10;

void setToEEPROM(int address, int value)
{
    EEPROM.write(address, value);
    EEPROM.commit();
}

void InternetConnection::callThermostatControll()
{
    MetheoData metheoData = MetheoData();
    metheoData.setData();
    ThermostatStatus status = Thermostat::controllThermostat(metheoData);

    InternetConnection::setStatusToBlynk(status.message, status.color);
    InternetConnection::setIsHeatingToBlynk(status.isHeating);
}

// Enable/disable thermostat, set value to EEPROM to address 1
BLYNK_WRITE(V0)
{
    param.asInt() ? setToEEPROM(1, true) : setToEEPROM(1, false);
    InternetConnection::callThermostatControll();
}

// Set temperature slider, write back to blynk to confirm show
BLYNK_WRITE(V10)
{
    int requiredTemp = param.asInt();
    Blynk.virtualWrite(V8, requiredTemp);
    Serial.println("Target Temperature is " + String(requiredTemp) + "°C");
    setToEEPROM(2, requiredTemp);
    InternetConnection::callThermostatControll();
}

// Static method - send message status to Blynk
void InternetConnection::setStatusToBlynk(String status, String color)
{
    Blynk.virtualWrite(V9, status);
    Blynk.setProperty(V9, "color", color);
}

// Send isHeating status to Blynk
void InternetConnection::setIsHeatingToBlynk(bool isHeating)
{
    Blynk.virtualWrite(V11, isHeating ? 1 : 0);
}

// Initialize WiFi connection and ThingSpeak. Return true if connection is sucessfull.
bool InternetConnection::initializeThingSpeak(void)
{
    WiFi.begin(ssid, password);

    Serial.print("WiFi connecting to: ");
    Serial.println(ssid);
    int i = 0;
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
        if (i == timeout)
        {
            Serial.println("Timeout on WiFi connection");
            return false;
        }
        i++;
    }
    Serial.println("");
    Serial.println("WiFi connected");

    ThingSpeak.begin(client);
    return true;
}

// Initialize WiFi connection and Blynk. Return true if connection is sucessfull.
bool InternetConnection::initializeBlynk(void)
{
    Serial.println("WiFi connecting to Blynk");
    Blynk.config(blynkAuth);

    // timeout 3sec
    Blynk.connect(1000);

    if (Blynk.connected())
    {
        Blynk.run();
    }

    Serial.println(Blynk.connected() ? "Blynk connected" : "Timeout on or internet connection");
    return Blynk.connected();
}

void InternetConnection::runBlynk(void)
{
    Blynk.run();
}

void InternetConnection::setMeteoDataToThingSpeakObject(MetheoData metheoData)
{
    // create data to send to ThingSpeak
    ThingSpeak.setField(1, metheoData.shtTemperature);
    ThingSpeak.setField(2, metheoData.shtHumidity);
}

bool InternetConnection::sendDataToThingSpeakApi(void)
{
    // Send data in one API call
    int status = ThingSpeak.writeFields(thingSpeakChannelId, thingSpeakWriteApiKey);
    if (status == OK_SUCCESS)
    {
        Serial.println("Send data to Thingspeak OK");
    }
    else
    {
        Serial.print("Error during sending data to ThingSpeak, status code: ");
        Serial.println(status);
    }
    return status;
}

bool InternetConnection::sendDataToBlynk(MetheoData metheoData)
{
    if (Blynk.connected())
    {
        Blynk.virtualWrite(V1, metheoData.shtTemperature);
        Blynk.virtualWrite(V2, metheoData.shtHumidity);
        Serial.println("Send data to Blynk OK");
        Blynk.run();
        return true;
    }
    else
    {
        Serial.println("Blynk is not connected.");
        return false;
    }
}
