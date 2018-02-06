#include "OledDisplay.h"

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

    // temperature SHT
    display.println("Ts:" + String(data.shtTemperature) + " C");
    // temperature BMP
    display.println("Tb:" + String(data.bmpTemperature) + " C");
    // temperature average
    display.println("Ta:" + String(data.averageTemperature) + " C");
    // humidity
    display.println("H:" + String(data.shtHumidity) + " %");
    // presure
    display.println("P:" + String(data.bmpPresure) + " h");

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