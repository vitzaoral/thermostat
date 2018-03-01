#include <Arduino.h>
#include <MetheoData.h>
#include <OledDisplay.h>
#include <InternetConnection.h>
#include <Ticker.h>
#include <EEPROM.h>

const int relayPinAddress = D5;
const int readMetheoDataDisplayDataControllThermostatInterval = 10000;
const int sendDataToInternetInterval = 60000;

const int redLed = D6;
const int greenLed = D7;
const int blueLed = D8;

MetheoData metheoData;
OledDisplay oledDisplay;
InternetConnection connection;

Ticker timerReadDataDisplayDataControllThermostat;
Ticker timerSendDataToInternet;

// Connections to APIs are OK
bool apisAreConnected = false;

void turnOffLed()
{
    // RGB LED is Anode type, so HIGH = turnOff, LOW = turnOn
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, HIGH);
    digitalWrite(blueLed, HIGH);
}

void setupRGBLed()
{
    pinMode(redLed, OUTPUT);
    pinMode(greenLed, OUTPUT);
    pinMode(blueLed, OUTPUT);

    turnOffLed();
}

void setLed(int ledPin, bool isTurnOn)
{
    turnOffLed();
    digitalWrite(ledPin, isTurnOn ? LOW : HIGH);
}

// Set thermostat ON/OFF
void controllThermostat(MetheoData data)
{
    // TODO: refactor, asi strcit do InternetConnection :-/
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
                InternetConnection::setStatusToBlynk("Heating ON", "#00FF00");
                connection.setIsHeatingToBlynk(true);
            }
            else
            {
                setLed(blueLed, true);
                digitalWrite(relayPinAddress, LOW);
                InternetConnection::setStatusToBlynk("Heating OFF", "#FF0000");
                connection.setIsHeatingToBlynk(false);
            }
        }
        else
        {
            setLed(redLed, true);
            digitalWrite(relayPinAddress, LOW);
            InternetConnection::setStatusToBlynk("Heating not enabled", "#FF0000");
            connection.setIsHeatingToBlynk(false);
        }
    }
    else
    {
        setLed(redLed, true);
        digitalWrite(relayPinAddress, LOW);
        InternetConnection::setStatusToBlynk("Data are invalid, heating OFF.", "#FF0000");
        connection.setIsHeatingToBlynk(false);
    }
}

void readMetheoDataDisplayDataControllThermostat()
{
    metheoData.setData();
    oledDisplay.printMetheoDataToDisplay(metheoData);
    controllThermostat(metheoData);
}

void sendDataToInternet()
{
    if (apisAreConnected)
    {
        connection.setMeteoDataToThingSpeakObject(metheoData);
        connection.sendDataToThingSpeakApi();
        connection.sendDataToBlynk(metheoData);
        Serial.println("Data was sended");
    }
}

void initializeInternetConnection()
{
    if (connection.initializeThingSpeak())
    {
        apisAreConnected = connection.initializeBlynk();
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

    pinMode(relayPinAddress, OUTPUT);
    setupRGBLed();
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