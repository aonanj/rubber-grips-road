#pragma once

#include <random>

// Simple pseudo sensors used to exercise the microcontroller logic on a desktop build.
class InfraredSensor {
public:
    explicit InfraredSensor(double nominalF = 140.0);
    double readF();

private:
    std::default_random_engine rng_;
    std::normal_distribution<double> dist_;
};

class AmbientSensor {
public:
    explicit AmbientSensor(double nominalF = 72.0);
    double readF();

private:
    std::default_random_engine rng_;
    std::normal_distribution<double> dist_;
};
