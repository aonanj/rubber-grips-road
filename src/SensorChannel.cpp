#include "SensorChannel.h"

#include <algorithm>

SensorChannel::SensorChannel(const char* name, SensorConfig config)
    : name_(name), config_(config), nextUpdateMs_(0) {}

std::optional<double> SensorChannel::ingest(double sampleF, uint32_t nowMs) {
    if (isOutlier(sampleF)) {
        return std::nullopt;
    }

    const double smoothed = smooth(sampleF);
    smoothedValue_ = smoothed;

    const int32_t timeToNextUpdate = static_cast<int32_t>(nowMs - nextUpdateMs_);
    if (timeToNextUpdate < 0) {
        return std::nullopt;
    }

    nextUpdateMs_ = nowMs + config_.updatePeriodMs;
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
