#include "SimulationSensors.h"

#include <chrono>
#include <random>

namespace {
unsigned seedFromClock() {
    return static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count());
}
}  // namespace

InfraredSensor::InfraredSensor(double nominalF)
    : rng_(seedFromClock()), dist_(nominalF, 6.0) {}

double InfraredSensor::readF() {
    return dist_(rng_);
}

AmbientSensor::AmbientSensor(double nominalF)
    : rng_(seedFromClock()), dist_(nominalF, 2.5) {}

double AmbientSensor::readF() {
    return dist_(rng_);
}
