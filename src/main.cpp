#include "Display.h"
#include "SensorChannel.h"
#include "SimulationSensors.h"

#include <chrono>
#include <iostream>
#include <thread>

namespace {
DisplayColor tireColorFor(double f) {
    if (f <= 90.0) {
        return DisplayColor::Red;
    }
    if (f <= 130.0) {
        return DisplayColor::Yellow;
    }
    if (f <= 185.0) {
        return DisplayColor::Green;
    }
    return DisplayColor::Red;
}

DisplayColor ambientColorFor(double f) {
    if (f <= 40.0) {
        return DisplayColor::Red;
    }
    if (f <= 60.0) {
        return DisplayColor::Yellow;
    }
    if (f <= 100.0) {
        return DisplayColor::Green;
    }
    return DisplayColor::Red;
}
}  // namespace

int main() {
    using namespace std::chrono_literals;

    SensorConfig tireConfig{
        .minValueF = 30.0,
        .maxValueF = 250.0,
        .smoothingFactor = 0.35,
        .updatePeriod = std::chrono::milliseconds(500),
    };

    SensorConfig ambientConfig{
        .minValueF = -20.0,
        .maxValueF = 140.0,
        .smoothingFactor = 0.25,
        .updatePeriod = std::chrono::milliseconds(1000),
    };

    SensorChannel tireChannel("Front Tire Surface", tireConfig);
    SensorChannel ambientChannel("Ambient Air", ambientConfig);

    InfraredSensor tireSensor(140.0);
    AmbientSensor ambientSensor(72.0);

    Display display;
    DisplayLine tireLine{"Front Tire Surface", 0.0, DisplayColor::Red};
    DisplayLine ambientLine{"Ambient Air", 0.0, DisplayColor::Red};
    bool tireInitialized = false;
    bool ambientInitialized = false;

    constexpr auto samplePeriod = 100ms;

    std::cout << "Starting tire temperature monitor... Press Ctrl+C to stop." << std::endl;

    while (true) {
        const auto now = std::chrono::steady_clock::now();

        if (auto value = tireChannel.ingest(tireSensor.readF(), now)) {
            tireLine.valueF = *value;
            tireLine.color = tireColorFor(*value);
            tireInitialized = true;
        }

        if (auto value = ambientChannel.ingest(ambientSensor.readF(), now)) {
            ambientLine.valueF = *value;
            ambientLine.color = ambientColorFor(*value);
            ambientInitialized = true;
        }

        if (tireInitialized && ambientInitialized) {
            display.render(tireLine, ambientLine);
        }

        std::this_thread::sleep_for(samplePeriod);
    }
}
