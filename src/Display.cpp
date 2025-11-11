#include "Display.h"

#include <cstring>

namespace {
constexpr uint16_t kDefaultWidth = 240;
constexpr uint16_t kDefaultHeight = 240;
constexpr uint8_t kLabelTextSize = 2;
constexpr uint8_t kValueTextSize = 3;
constexpr uint8_t kPadding = 6;

constexpr uint16_t rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return static_cast<uint16_t>(((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3));
}
}  // namespace

Display::Display(uint8_t csPin, uint8_t dcPin, uint8_t rstPin, uint16_t width, uint16_t height)
    : tft_(csPin, dcPin, rstPin),
      width_(width == 0 ? kDefaultWidth : width),
      height_(height == 0 ? kDefaultHeight : height),
      lineHeight_((height_ < 2) ? height_ : height_ / 2),
      initialized_(false) {}

void Display::begin() {
    if (initialized_) {
        return;
    }
    tft_.init(width_, height_);
    tft_.setRotation(2);
    tft_.fillScreen(ST77XX_BLACK);
    tft_.setTextWrap(false);
    initialized_ = true;
}

void Display::render(const DisplayLine& tireLine, const DisplayLine& ambientLine) {
    if (!initialized_) {
        begin();
    }
    drawLine(tireLine, 0);
    drawLine(ambientLine, lineHeight_);
}

void Display::drawLine(const DisplayLine& line, uint16_t yOffset) {
    const uint16_t bg = colorFor(line.color);
    const uint16_t textColor = textColorFor(line.color);
    tft_.fillRect(0, yOffset, width_, lineHeight_, bg);

    tft_.setTextColor(textColor);
    tft_.setCursor(kPadding, yOffset + kPadding);
    tft_.setTextSize(kLabelTextSize);
    tft_.print(line.label);

    char formatted[12];
    formatTemperature(line.valueF, formatted, sizeof(formatted));
    tft_.setTextSize(kValueTextSize);
    tft_.setCursor(kPadding, yOffset + (lineHeight_ / 2));
    tft_.print(formatted);
}

uint16_t Display::colorFor(DisplayColor color) const {
    switch (color) {
        case DisplayColor::Red:
            return rgb565(220, 60, 60);
        case DisplayColor::Yellow:
            return rgb565(255, 215, 0);
        case DisplayColor::Green:
            return rgb565(30, 160, 110);
    }
    return ST77XX_BLACK;
}

uint16_t Display::textColorFor(DisplayColor color) const {
    switch (color) {
        case DisplayColor::Red:
            return ST77XX_WHITE;
        case DisplayColor::Yellow:
        case DisplayColor::Green:
            return ST77XX_BLACK;
    }
    return ST77XX_WHITE;
}

void Display::formatTemperature(double valueF, char* buffer, size_t len) const {
    if (!buffer || len < 3) {
        return;
    }

    char raw[16];
    dtostrf(valueF, 5, 1, raw);

    size_t start = 0;
    while (raw[start] == ' ' && raw[start] != '\0') {
        ++start;
    }

    size_t idx = 0;
    while (raw[start] != '\0' && idx < len - 2) {
        buffer[idx++] = raw[start++];
    }

    buffer[idx++] = 'F';
    buffer[idx] = '\0';
}
