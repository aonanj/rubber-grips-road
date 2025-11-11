#pragma once

#include <cstdint>
#include <optional>

struct SensorConfig {
    double minValueF;                               // minimum plausible reading in Fahrenheit
    double maxValueF;                               // maximum plausible reading in Fahrenheit
    double smoothingFactor;                         // exponential smoothing factor (alpha)
    uint32_t updatePeriodMs;                        // how often to push new values to the LCD
};

class SensorChannel {
public:
    SensorChannel(const char* name, SensorConfig config);

    // Returns a new smoothed value when the channel emits an update; std::nullopt otherwise.
    std::optional<double> ingest(double sampleF, uint32_t nowMs);

    [[nodiscard]] std::optional<double> lastValue() const { return smoothedValue_; }
    [[nodiscard]] const char* name() const { return name_; }

private:
    bool isOutlier(double sampleF) const;
    double smooth(double sampleF);

    const char* name_;
    SensorConfig config_;
    std::optional<double> smoothedValue_;
    uint32_t nextUpdateMs_;
};
