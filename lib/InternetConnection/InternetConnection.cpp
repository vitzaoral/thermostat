#include "InternetConnection.h"
#include "../../src/settings.cpp"
#include <BlynkSimpleEsp8266.h>
#include <EEPROM.h>

WiFiClient client;
Settings settings;

const char *thingSpeakWriteApiKey = settings.thingSpeakWriteApiKey;
const unsigned long thingSpeakChannelId = settings.thingSpeakChannelId;
const char *ssid = settings.ssid;
const char *password = settings.password;
const char *blynkAuth = settings.blynkAuth;

// number of attempts to connecting WIFI, API etc.
const int timeout = 10;

int relayPin;

void setStatusToBlynk(String status, String color)
{
    Blynk.virtualWrite(V9, status);
    Blynk.setProperty(V9, "color", color);
}

void setToEEPROM(int address, int value)
{
    EEPROM.write(address, value); 
    EEPROM.commit(); 
}

// Enable/disable relay using virt pin 0
BLYNK_WRITE(0)
{
    if (param.asInt())
    {
        digitalWrite(relayPin, HIGH);
        setStatusToBlynk("Heating ON", "#00FF00");
        setToEEPROM(1, 1);
    }
    else
    {
        digitalWrite(relayPin, LOW);
        setStatusToBlynk("Heating OFF", "#FF0000");
        setToEEPROM(1, 0);
    }
}

// Set temperature slider, write back to blynk to confirm show
BLYNK_WRITE(V10)
{
    int requiredTemp = param.asInt();
    Blynk.virtualWrite(V8, requiredTemp);
    Serial.println("Target Temperature is " + String(requiredTemp) + "°C");
    setToEEPROM(2, requiredTemp);
}

// Initialize and get metheorological data
InternetConnection::InternetConnection(int relayPinAddress)
{
    relayPin = relayPinAddress;
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
    Blynk.begin(blynkAuth, ssid, password);
    int i = 0;

    while (Blynk.connect() != 1)
    {
        delay(500);
        Serial.print(".");
        if (i == timeout)
        {
            Serial.println("Timeout on Blynk connection");
            return false;
        }
        i++;
    }

    Serial.println("Blynk connected");
    return true;
}

void InternetConnection::runBlynk(void)
{
    Blynk.run();
}

void InternetConnection::setMeteoDataToThingSpeakObject(MetheoData metheoData)
{
    // create data to send to ThingSpeak
    ThingSpeak.setField(1, metheoData.shtTemperature);
    ThingSpeak.setField(2, metheoData.bmpTemperature);
    ThingSpeak.setField(3, metheoData.bmpPresure);
    ThingSpeak.setField(4, metheoData.shtHumidity);
}

bool InternetConnection::sendDataToThingSpeakApi(void)
{
    // Send data in one API call
    // TODO: co se stane pokud sluzba zrovna nejede, vypadne WIFi apod.
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

void InternetConnection::sendDataToBlynk(MetheoData metheoData)
{
    // create data to send to Blynk. For some reason need to call connect.
    if (Blynk.connect() == 1)
    {
        Blynk.run();
        Blynk.virtualWrite(V1, metheoData.shtTemperature);
        Blynk.virtualWrite(V2, metheoData.bmpTemperature);
        Blynk.virtualWrite(V3, metheoData.shtHumidity);
        Blynk.virtualWrite(V4, metheoData.bmpPresure);
        Serial.println("Send data to Blynk OK");
    }
    else
    {
        Serial.println("Error during sending data to Blynk");
    }
}
