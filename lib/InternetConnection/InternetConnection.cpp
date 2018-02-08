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

void setToEEPROM(int address, int value)
{
    EEPROM.write(address, value); 
    EEPROM.commit(); 
}

// Enable/disable thermostat, set value to EEPROM to address 1
BLYNK_WRITE(0)
{
    // TODO: slo by okamzite pinkout metodu controllThermostat z mainu? Asi jedine presunout tu metodu sem :-/
    param.asInt() ? setToEEPROM(1, true) :  setToEEPROM(1, false);
}

// Set temperature slider, write back to blynk to confirm show
BLYNK_WRITE(V10)
{
    // TODO: slo by okamzite pinkout metodu controllThermostat z mainu?
    int requiredTemp = param.asInt();
    Blynk.virtualWrite(V8, requiredTemp);
    Serial.println("Target Temperature is " + String(requiredTemp) + "°C");
    setToEEPROM(2, requiredTemp);
}

// Send message status to Blynk
void InternetConnection::setStatusToBlynk(String status, String color)
{
    Blynk.virtualWrite(V9, status);
    Blynk.setProperty(V9, "color", color);
}

// Send isHeating status to Blynk
void InternetConnection::setIsHeatingToBlynk(bool isHeating)
{
    Blynk.virtualWrite(V11, isHeating ? 1 : 0);
   // Blynk.setProperty(V11, "color", isHeating ? "#00FF00" : "#FF0000");
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
    // TODO: problem behu donekonecna, pokud mam spatny klic (pokud nefunguje Blynk tak mozna ne). Napsat na forum Blynku.
    Serial.println("WiFi connecting to Blynk");
    Blynk.begin(blynkAuth, ssid, password);
    Blynk.run();
    
    Serial.println(Blynk.connected() ? "Blynk connected" : "Timeout on Blynk");
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
    if (Blynk.connect())
    {
        Blynk.virtualWrite(V1, metheoData.shtTemperature);
        Blynk.virtualWrite(V2, metheoData.shtHumidity);
        Serial.println("Send data to Blynk OK");
        Blynk.run();
    }
    else
    {
        Serial.println("Blynk is not connected.");
    }
}
