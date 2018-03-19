#include <Arduino.h>
#include <InternetConnection.h>
#include <MetheoData.h>
#include <OledDisplay.h>
#include <Thermostat.h>
#include <Ticker.h>
#include <EEPROM.h>

const int readMetheoDataDisplayDataControllThermostatInterval = 10000;
const int sendDataToInternetInterval = 60000;

MetheoData metheoData;
OledDisplay oledDisplay;
InternetConnection connection;

Ticker timerReadDataDisplayDataControllThermostat;
Ticker timerSendDataToInternet;

// Connections to APIs are OK
bool apisAreConnected = false;

void readMetheoDataDisplayDataControllThermostat()
{
    metheoData.setData();
    oledDisplay.printMetheoDataToDisplay(metheoData);
    Thermostat::controllThermostat(metheoData);
}

void initializeInternetConnection()
{
    if (connection.initializeThingSpeak())
    {
        apisAreConnected = connection.initializeBlynk();
    }
}

void sendDataToInternet()
{
    if (apisAreConnected)
    {
        connection.setMeteoDataToThingSpeakObject(metheoData);
        bool successThingSpeak = connection.sendDataToThingSpeakApi();
        bool successBlynk = connection.sendDataToBlynk(metheoData);

        if (successThingSpeak && successBlynk) {
            Serial.println("Data was sent");
        }
        else {
            Serial.println("No internet connection, try initialize connection");
            apisAreConnected = false;
            initializeInternetConnection();
        }
    }
    else {
        initializeInternetConnection();
    }
}

void setupTimers()
{
    timerReadDataDisplayDataControllThermostat.setCallback(readMetheoDataDisplayDataControllThermostat);
    timerReadDataDisplayDataControllThermostat.setInterval(readMetheoDataDisplayDataControllThermostatInterval);
    timerReadDataDisplayDataControllThermostat.start();

    timerSendDataToInternet.setCallback(sendDataToInternet);
    timerSendDataToInternet.setInterval(sendDataToInternetInterval);
    timerSendDataToInternet.start();
}

// Set up environment before loop
void setup()
{
    // Two bytes for device status (enabled/disabled, required temperature)
    EEPROM.begin(2);
    // TODO: vyzkouset OTA
    Serial.begin(9600);
    delay(100);

    Thermostat::initialize();
    initializeInternetConnection();
    setupTimers();
}

// Excecute code in forever loop
void loop()
{
    timerReadDataDisplayDataControllThermostat.update();
    timerSendDataToInternet.update();
    connection.runBlynk();
}