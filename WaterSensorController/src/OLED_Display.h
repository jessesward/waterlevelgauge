#pragma once
// OLED_Display.h

#ifndef OLED_DISPLAY_H
#define OLED_DISPLAY_H

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

namespace user
{
    class OLED_Display
    {
    private:
        Adafruit_SSD1306 oled;

    public:
        void init(int SDA, int SLC);
        void write(String text);
        // Other methods can be added here for further functionalities
    };
}

#endif
