#include "OledDisplay.h"
#include <EEPROM.h>

Adafruit_SSD1306 display(OLED_RESET);

// Initialize OLED Shield 64x48
OledDisplay::OledDisplay()
{
    // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C (for the 64x48)
    display.display();
}

void OledDisplay::printMetheoDataToDisplay(MetheoData data)
{
    prepareDisplay();

    // temperature
    display.println();
    display.println("T:" + String(data.shtTemperature) + " C");
    // humidity
    display.println("H:" + String(data.shtHumidity) + " %");
    
    // TODO: EEPROM konstanty 1 a 2, nejak ucesat, i na jinych mistech
    display.println();
    display.println(EEPROM.read(1) ? "ON" : "OFF");
    display.println("Set: " + String(EEPROM.read(2)) + "C");

    display.display();
}

// clear display, set size and cursor
void OledDisplay::prepareDisplay(void)
{
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
}