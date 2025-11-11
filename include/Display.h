#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Arduino.h>
#include <SPI.h>
#include <cstddef>

enum class DisplayColor { Red, Yellow, Green };

struct DisplayLine {
    const char* label;
    double valueF;
    DisplayColor color;
};

class Display {
public:
    Display(uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint16_t width = 240, uint16_t height = 240);

    void begin();
    void render(const DisplayLine& tireLine, const DisplayLine& ambientLine);

private:
    void drawLine(const DisplayLine& line, uint16_t yOffset);
    uint16_t colorFor(DisplayColor color) const;
    uint16_t textColorFor(DisplayColor color) const;
    void formatTemperature(double valueF, char* buffer, size_t len) const;

    Adafruit_ST7789 tft_;
    uint16_t width_;
    uint16_t height_;
    uint16_t lineHeight_;
    bool initialized_;
};
