# Rubber Grips Road
Embedded firmware for the Rubber Grips Road motorcycle accessory. The sketch targets the
Arduino Nano Every and drives:
- A MLX90614 infrared (IR) sensor for front-tire surface temperature (I²C on A4/A5).
- A BME280 for ambient temperature (shares the I²C bus).
- An SPI color LCD (Adafruit ST77xx-compatible) showing color-coded status lines.

## Prototype Description
* **Goal**: Provide a motorcycle operator with continuously updated information about tire surface temperature and lean angle safety. 
* Major components:
  1. **IR temperature sensor**
    * Directed to the front tire of the motorcycle in order to detect the surface temperature of the front tire. Suggested mounting locations: (1)radiator guard, (2) exterior surface of front-most outer fairing, (3) fork tube (if line-of-sight to tire surface), (4) exterior surface of cowl. 
  2. **Ambient temperature sensor**
    * Preferably mounted at a location (e.g., interior surface of an outer fairing) that minimizes the influence of engine and exhaust heat and wind on ambient temperature readings. 
  3. **Microcontroller** (suggested: Arduino Nano Every)
    * Preferably mounted on an interior surface of a fairing or behind the LCD display. 
  4. **LCD display**
    * Preferably mounted on the handlebars or handlebar clamp. 
* **Operational Workflow**:
  1. Outputs of the IR temperature sensor and ambient temperature sensor are input into the Arduino microcontroller. 
  2. Microcontroller outputs a first value and a second value to display on the LCD display. 
     * The first value is the indicative of the surface temperature at which the IR temperature sensor is directed. 
     * The second value is indicative of the ambient temperature of the motorcycle’s current operating environment. 
  3. Microcontroller calculates the first value (updates every 0.5 seconds). 
     1. Filters out outlier samples that are either less than a reasonable minimum surface temperature threshold or greater than a maximum reasonable surface temperature threshold.
     2. Applies exponential smoothing that gives weight to new IR temperature sensor readings while retaining memory of past IR temperature sensor readings. 
  4. Microcontroller calculates the second value (updates every 1 second).
     1. Filters out outlier samples that are either less than a reasonable minimum ambient temperature threshold or greater than a maximum reasonable ambient temperature threshold.
     2. Applies exponential smoothing that gives weight to new ambient temperature readings while retaining memory of past ambient temperature readings. 
  5. Microcontroller instructs the LCD display to present the first value. First value presented in one of: 
     * **Red**: 90℉ ≤  first value
       * Indicates tire grip at the contact patch is too low to maintain traction during countersteering or emergency braking.
     * **Yellow**: 90℉ < first value ≤ 130℉ 
       * Indicates conservative counter steering may be utilized, but high risk of traction loss at significant lean angles or abrupt emergency braking.
     * **Green**: 130℉ < first value ≤ 185℉
       * Indicates optimal surface temperature to maintain tire grip at the contact patch.
     * **Red**: 185℉ < first value
       * Indicates tire surface temperature rapidly approaching overheating at which tire grip at the contact patch is again reduced. 
  6. Microcontroller instructs the LCD display to present the second value. Second value presented in one of: 
     * **Red**: 40℉ ≤  second value
       * Indicates ambient temperature is low enough that tire grip at the contact patch is significantly reduced and traction loss is highly likely at most lean angles.
     * **Yellow**: 40℉ < second value ≤ 60℉ 
       * Indicates ambient temperature is low enough to increase the likelihood of traction loss at significant lean angles.
     * **Green**: 60℉ < second value ≤ 100℉
       * Indicates optimal ambient temperature to maintain tire grip at the contact patch.
     * **Red**: 100℉ < second value
       * Indicates ambient temperature has reached a level high enough to cause air expansion inside the tires, which may reduce the size of the contact patch. 

## Hardware Assumptions

| Function              | Nano Every Pin | Peripheral Pin        |
| --------------------- | -------------- | --------------------- |
| I²C SDA               | A4             | MLX90614 SDA, BME280 SDA |
| I²C SCL               | A5             | MLX90614 SCL, BME280 SCL |
| LCD SPI Clock         | D13 (SCK)      | Display CLK           |
| LCD SPI MOSI          | D11 (MOSI)     | Display DIN/MOSI      |
| LCD Chip Select       | D10            | Display CS            |
| LCD Data/Command      | D9             | Display DC            |
| LCD Reset             | D8             | Display RST           |

Power the sensors according to their datasheets (5 V tolerant variants are
recommended for the Nano Every).

## Firmware Logic

- Samples both sensors every 100 ms.
- Rejects samples outside configurable bounds before they ever affect the UI.
- Applies exponential smoothing to preserve short-term history.
- Updates the tire line every 500 ms and the ambient line every 1000 ms.
- Colors follow the same thresholds used in the desktop prototype.
- Draws two large text tiles on the LCD so the rider can glance at the status.

Logic Highlights:

- `SensorChannel` — filtering, smoothing, and update-period management.
- `Display` — wraps an Adafruit ST77xx display over hardware SPI.
- `src/main.cpp` — Arduino `setup/loop`, real sensor drivers, and thresholds.
  - Smoothing constants and thresholds are configurable via `src/main.cpp` to fit
  different tire compounds and environments. 

## Required Arduino Libraries

Install these libraries through the Arduino IDE Library Manager or `arduino-cli
lib install`:

- Adafruit MLX90614 Library
- Adafruit BME280 Library (installs Adafruit Unified Sensor & BusIO as deps)
- Adafruit GFX Library
- Adafruit ST7735 and ST7789 Library

## Building & Flashing

1. Install the “Arduino megaAVR” board package and select **Arduino Nano Every**.
2. Open this folder in the Arduino IDE _or_ use the CLI:

   ```bash
   arduino-cli compile --fqbn arduino:megaavr:nona4809
   arduino-cli upload --fqbn arduino:megaavr:nona4809 -p /dev/ttyUSB0
   ```

3. Open the Serial Monitor at 115200 baud to watch basic boot diagnostics.

The sketch initializes both sensors, paints the display once, and then refreshes
any time one of the channels produces a new filtered value.

## Notes & Troubleshooting

- If either sensor fails to initialize, the firmware halts and prints an error
  over Serial so the issue is obvious during bench testing.
- The BME280 is probed at addresses `0x76` and `0x77`; adjustable via `src/main.cpp`
  for custom breakouts.

## License

This repository is publicly viewable for portfolio purposes only. The code is proprietary.
Copyright © 2025 Phaethon Order LLC. All rights reserved.
Contact [support@phaethon.llc](mailto:support@phaethon.llc) for licensing or reuse requests.

See [LICENSE](LICENSE)

## Contact
Questions or support: [support@phaethon.llc](mailto:support@phaethon.llc).