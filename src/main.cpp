#include <Adafruit_BME280.h>
#include <Adafruit_MLX90614.h>
#include <Arduino.h>
#include <Wire.h>

#include "Display.h"
#include "SensorChannel.h"

namespace {
constexpr uint8_t PIN_TFT_CS = 10;
constexpr uint8_t PIN_TFT_DC = 9;
constexpr uint8_t PIN_TFT_RST = 8;

constexpr uint32_t SAMPLE_PERIOD_MS = 100;

Adafruit_MLX90614 tireSensor = Adafruit_MLX90614();
Adafruit_BME280 ambientSensor;

SensorConfig tireConfig{
    .minValueF = 30.0,
    .maxValueF = 250.0,
    .smoothingFactor = 0.35,
    .updatePeriodMs = 500,
};

SensorConfig ambientConfig{
    .minValueF = -20.0,
    .maxValueF = 140.0,
    .smoothingFactor = 0.25,
    .updatePeriodMs = 1000,
};

SensorChannel tireChannel("Front Tire Surface", tireConfig);
SensorChannel ambientChannel("Ambient Air", ambientConfig);

Display display(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
DisplayLine tireLine{"Front Tire Surface", 0.0, DisplayColor::Red};
DisplayLine ambientLine{"Ambient Air", 0.0, DisplayColor::Red};

bool tireInitialized = false;
bool ambientInitialized = false;

double celsiusToF(double celsius) {
    return celsius * 9.0 / 5.0 + 32.0;
}

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

double readTireTempF() {
    return celsiusToF(tireSensor.readObjectTempC());
}

double readAmbientTempF() {
    return celsiusToF(ambientSensor.readTemperature());
}
}  // namespace

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 2000) {
        delay(10);
    }

    Serial.println(F("Rubber Grips Road - Nano Every"));
    Wire.begin();  // SDA=A4, SCL=A5 on the Nano Every

    if (!tireSensor.begin()) {
        Serial.println(F("ERROR: MLX90614 infrared sensor not detected."));
        while (true) {
            delay(500);
        }
    }

    if (!ambientSensor.begin(0x76) && !ambientSensor.begin(0x77)) {
        Serial.println(F("ERROR: BME280 ambient sensor not detected."));
        while (true) {
            delay(500);
        }
    }

    display.begin();
    display.render(tireLine, ambientLine);
}

void loop() {
    const uint32_t now = millis();
    bool updated = false;

    if (auto value = tireChannel.ingest(readTireTempF(), now)) {
        tireLine.valueF = *value;
        tireLine.color = tireColorFor(*value);
        tireInitialized = true;
        updated = true;
    }

    if (auto value = ambientChannel.ingest(readAmbientTempF(), now)) {
        ambientLine.valueF = *value;
        ambientLine.color = ambientColorFor(*value);
        ambientInitialized = true;
        updated = true;
    }

    if (updated && tireInitialized && ambientInitialized) {
        display.render(tireLine, ambientLine);
    }

    delay(SAMPLE_PERIOD_MS);
}
