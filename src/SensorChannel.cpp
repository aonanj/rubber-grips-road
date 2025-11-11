#include "SensorChannel.h"

#include <algorithm>
#include <optional>

SensorChannel::SensorChannel(std::string name, SensorConfig config)
    : name_(std::move(name)), config_(config), nextUpdate_(std::chrono::steady_clock::time_point::min()) {}

std::optional<double> SensorChannel::ingest(
    double sampleF, std::chrono::steady_clock::time_point now) {
    if (isOutlier(sampleF)) {
        return std::nullopt;
    }

    const double smoothed = smooth(sampleF);
    smoothedValue_ = smoothed;

    if (now < nextUpdate_) {
        return std::nullopt;
    }

    nextUpdate_ = now + config_.updatePeriod;
    return smoothedValue_;
}

bool SensorChannel::isOutlier(double sampleF) const {
    return sampleF < config_.minValueF || sampleF > config_.maxValueF;
}

double SensorChannel::smooth(double sampleF) {
    if (!smoothedValue_) {
        return sampleF;
    }

    const double alpha = std::clamp(config_.smoothingFactor, 0.0, 1.0);
    return alpha * sampleF + (1.0 - alpha) * smoothedValue_.value();
}
