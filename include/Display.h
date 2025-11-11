#pragma once

#include <string>

enum class DisplayColor { Red, Yellow, Green };

struct DisplayLine {
    std::string label;
    double valueF;
    DisplayColor color;
};

class Display {
public:
    void render(const DisplayLine& tireLine, const DisplayLine& ambientLine) const;

private:
    static std::string colorToString(DisplayColor color);
};
