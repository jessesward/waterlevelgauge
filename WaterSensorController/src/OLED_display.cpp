// OLED_Display.cpp
#include "OLED_Display.h"

#define SCREEN_WIDTH 128 // OLED display width,  in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels
#define TEXT_SIZE 2
// declare an SSD1306 display object connected to I2C
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

namespace user
{
    void OLED_Display::init(int SDA, int SLC)
    {
        Wire.begin(SDA, SLC); // Initialize the Wire library with defined pins
        // initialize OLED display with I2C address 0x3C
        if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C))
        {
            Serial.println(F("failed to start SSD1306 OLED"));
            while (1)
                ;
        }

        // delay(2000);                 // wait two seconds for initializing
    }

    void OLED_Display::write(String text)
    {

        oled.clearDisplay();         // clear display
        oled.setTextSize(TEXT_SIZE); // set text size
        oled.setTextColor(WHITE);    // set text color
        oled.setCursor(0, 15);       // set position to display
        oled.println(text);          // set text
        oled.display();              // display on OLED
    }
}