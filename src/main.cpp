#include <Arduino.h>
#include <MetheoData.h>
#include <OledDisplay.h>
#include <InternetConnection.h>
#include <Ticker.h>

const int relayPinAddress = D5;
const int readMetheoDataAndDisplayInterval = 10000;
const int sendDataToInternetInterval = 60000;

MetheoData metheoData;
OledDisplay oledDisplay;
InternetConnection connection(relayPinAddress);

Ticker timerReadDataAndDisplay;
Ticker timerSendDataToInternet;

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

void setupTimers() {
    timerReadDataAndDisplay.setCallback(readMetheoDataAndDisplay);
    timerReadDataAndDisplay.setInterval(readMetheoDataAndDisplayInterval);
    timerReadDataAndDisplay.start();

    timerSendDataToInternet.setCallback(sendDataToInternet);
    timerSendDataToInternet.setInterval(sendDataToInternetInterval);
    timerSendDataToInternet.start();
}

// Set up environment before loop
void setup()
{
    // TODO: vyzkouset OTA
    Serial.begin(9600);
    delay(100);
    
    pinMode(relayPinAddress, OUTPUT);
    initializeInternetConnection();
    setupTimers();
}

// Excecute code in forever loop
void loop()
{
    timerReadDataAndDisplay.update();
    timerSendDataToInternet.update();
    connection.runBlynk();
}