#include <Arduino.h>
#include <MetheoData.h>
#include <OledDisplay.h>
#include <InternetConnection.h>

MetheoData metheoData;
OledDisplay oledDisplay;
InternetConnection connection;

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
}

// Excecute code in forever loop
void loop()
{
    readMetheoDataAndDisplay();
    sendDataToInternet();
    delay(30000);
}