#include "Display.h"

#include <iomanip>
#include <iostream>

void Display::render(const DisplayLine& tireLine, const DisplayLine& ambientLine) const {
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "==============================\n";
    std::cout << tireLine.label << ": " << tireLine.valueF << " F (" << colorToString(tireLine.color)
              << ")\n";
    std::cout << ambientLine.label << ": " << ambientLine.valueF << " F ("
              << colorToString(ambientLine.color) << ")\n";
    std::cout << "==============================\n";
}

std::string Display::colorToString(DisplayColor color) {
    switch (color) {
        case DisplayColor::Red:
            return "RED";
        case DisplayColor::Yellow:
            return "YELLOW";
        case DisplayColor::Green:
            return "GREEN";
    }
    return "UNKNOWN";
}
