#pragma once

#include <chrono>
#include <optional>
#include <string>

struct SensorConfig {
    double minValueF;                               // minimum plausible reading in Fahrenheit
    double maxValueF;                               // maximum plausible reading in Fahrenheit
    double smoothingFactor;                         // exponential smoothing factor (alpha)
    std::chrono::milliseconds updatePeriod;         // how often to push new values to the LCD
};

class SensorChannel {
public:
    SensorChannel(std::string name, SensorConfig config);

    // Returns a new smoothed value when the channel emits an update; std::nullopt otherwise.
    std::optional<double> ingest(double sampleF,
                                 std::chrono::steady_clock::time_point now);

    [[nodiscard]] std::optional<double> lastValue() const { return smoothedValue_; }
    [[nodiscard]] const std::string& name() const { return name_; }

private:
    bool isOutlier(double sampleF) const;
    double smooth(double sampleF);

    std::string name_;
    SensorConfig config_;
    std::optional<double> smoothedValue_;
    std::chrono::steady_clock::time_point nextUpdate_;
};
