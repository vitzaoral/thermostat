#include <Arduino.h>
#include <Ticker.h>
#include <InternetConnection.h>
#include <MetheoData.h>
#include <OledDisplay.h>
#include <Thermostat.h>
#include <EEPROM.h>

const int readMetheoDataDisplayDataControllThermostatInterval = 10000;
const int sendDataToInternetInterval = 60000;

MetheoData metheoData;
OledDisplay oledDisplay;
InternetConnection connection;

void readMetheoDataDisplayDataControllThermostat();
void sendDataToInternet();
Ticker timerReadDataDisplayDataControllThermostat(readMetheoDataDisplayDataControllThermostat, readMetheoDataDisplayDataControllThermostatInterval);
Ticker timerSendDataToInternet(sendDataToInternet, sendDataToInternetInterval);

// Connections to APIs are OK
bool apisAreConnected = false;

void readMetheoDataDisplayDataControllThermostat()
{
    metheoData.setData();
    oledDisplay.printMetheoDataToDisplay(metheoData);
    ThermostatStatus status = Thermostat::controllThermostat(metheoData);

    InternetConnection::setStatusToBlynk(status.message, status.color);
    InternetConnection::setIsHeatingToBlynk(status.isHeating);
}

void initializeInternetConnection()
{
    if (connection.initializeThingSpeak())
    {
        apisAreConnected = connection.initializeBlynk();
        connection.initializeOTA();
    }
}

void sendDataToInternet()
{
    if (apisAreConnected)
    {
        connection.setMeteoDataToThingSpeakObject(metheoData);
        bool successThingSpeak = connection.sendDataToThingSpeakApi();
        bool successBlynk = connection.sendDataToBlynk(metheoData);

        if (successThingSpeak && successBlynk)
        {
            Serial.println("Data was sent");
        }
        else
        {
            Serial.println("No internet connection, try initialize connection");
            apisAreConnected = false;
            initializeInternetConnection();
        }
    }
    else
    {
        initializeInternetConnection();
    }
}

void startTimers()
{
    timerReadDataDisplayDataControllThermostat.start();
    timerSendDataToInternet.start();
}

void updateTimers()
{
    timerReadDataDisplayDataControllThermostat.update();
    timerSendDataToInternet.update();
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
    startTimers();
}

// Excecute code in forever loop
void loop()
{
    updateTimers();
    connection.runBlynk();
    connection.handleOTA();
}