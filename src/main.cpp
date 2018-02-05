#include <Arduino.h>
#include <MetheoData.h>
#include <OledDisplay.h>
#include <InternetConnection.h>
#include <Ticker.h>

MetheoData metheoData;
OledDisplay oledDisplay;
InternetConnection connection;

Ticker timerReadDataAndDisplay;
Ticker timerSendDataToInternet;

int relayPin = D5;

// Connections to APIs are OK
bool apisAreConnected = false;

void readMetheoDataAndDisplay()
{

    metheoData.setData();
    oledDisplay.printMetheoDataToDisplay(metheoData);
}

void sendDataToInternet()
{
    if (apisAreConnected)
    {
        connection.setMeteoDataToThingSpeakObject(metheoData);
        connection.sendDataToThingSpeakApi();
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

// Set up environment before loop
void setup()
{
    // TODO: vyzkouset OTA
    Serial.begin(9600);
    delay(100);
    pinMode(relayPin, OUTPUT);
    initializeInternetConnection();
    
    timerReadDataAndDisplay.setCallback(readMetheoDataAndDisplay);
    timerReadDataAndDisplay.setInterval(20000);
    timerReadDataAndDisplay.start();

    timerSendDataToInternet.setCallback(sendDataToInternet);
    timerSendDataToInternet.setInterval(60000);
    timerSendDataToInternet.start();
}

// Excecute code in forever loop
void loop()
{
    timerReadDataAndDisplay.update();
    timerSendDataToInternet.update();
    connection.runBlynk();
}