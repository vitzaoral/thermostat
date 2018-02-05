#include <Arduino.h>
#include <MetheoData.h>
#include <OledDisplay.h>
#include <InternetConnection.h>
#include <Ticker.h>

MetheoData metheoData;
OledDisplay oledDisplay;
InternetConnection connection;

Ticker timer1;
Ticker timer2;

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
    initializeInternetConnection();
    
    timer1.setCallback(readMetheoDataAndDisplay);
    timer1.setInterval(20000);
    timer1.start();

    timer2.setCallback(sendDataToInternet);
    timer2.setInterval(60000);
    timer2.start();
}

// Excecute code in forever loop
void loop()
{
    // readMetheoDataAndDisplay();
    // sendDataToInternet();
    //delay(30000);
    timer1.update();
    timer2.update();
}