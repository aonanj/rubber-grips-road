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
      * Directed to the front tire of the motorcycle in order to detect the surface temperature of the front tire. Suggested mounting locations: (1) radiator guard, (2) exterior surface of front-most outer fairing, (3) fork tube (if line-of-sight to tire surface), (4) exterior surface of cowl. 
  3. **Ambient temperature sensor**
      * Preferably mounted at a location that minimizes the influence of engine and exhaust heat and wind on ambient temperature readings. Suggested mounting locations: (1) interior surface of an outer fairing, (2) underside of seat (if sufficient distance from exhaust system/muffler), (3) subframe proximate to rear suspension.  
  3. **Microcontroller** (suggested: Arduino Nano Every)
      * Preferably mounted on an interior surface of a fairing or behind the LCD display. 
  4. **LCD display**
      * Preferably mounted on the handlebars, handlebar clamp, or steering stem nut. 
* **Operational Workflow**:
  1. Outputs of the IR temperature sensor and ambient temperature sensor are input into the Arduino microcontroller. 
  2. Microcontroller calculates and outputs values on a tire temp line and an ambient temp line to display on the LCD display. 
     * Tire temp line: carries values indicative of the temperature at the surface to which the IR temperature sensor is directed (i.e., front-tire surface temperature). 
     * Ambient temp line: carries values indicative of the ambient temperature of the motorcycle’s current operating environment. 
  3. Microcontroller calculates values for tire temp line every 500ms. 
     * Filters out outlier samples from IR temp sensor that are either less than a reasonable minimum surface temperature threshold or greater than a maximum reasonable surface temperature threshold.
     * Applies exponential smoothing that gives weight to new IR temperature sensor samples while retaining memory of past IR temperature sensor samples within a first sampling window. 
  4. Microcontroller calculates values for ambient temp line every 1000ms.
     * Filters out outlier samples from ambient temp sensor that are either less than a reasonable minimum ambient temperature threshold or greater than a maximum reasonable ambient temperature threshold.
     * Applies exponential smoothing that gives weight to new ambient temperature sensor samples while retaining memory of past ambient temperature sensor samples within a second sampling window.
     * Microcontroller staggers updates on tire temp line from updates on ambient temp line. 
  5. Microcontroller instructs the LCD display to display each value on tire temp line in one of: 
     * **Red**: 90℉ ≤  first value
       - Indicates tire grip at the contact patch is too low to maintain traction during countersteering or emergency braking.
     * **Yellow**: 90℉ < first value ≤ 130℉ 
       - Indicates conservative counter steering may be utilized, but high risk of traction loss at significant lean angles or abrupt emergency braking.
     * **Green**: 130℉ < first value ≤ 185℉
       - Indicates optimal surface temperature to maintain tire grip at the contact patch.
     * **Red**: 185℉ < first value
       - Indicates tire surface temperature rapidly approaching overheating at which tire grip at the contact patch is again reduced. 
  6. Microcontroller instructs the LCD display to display each value on ambient temp line in one of: 
     * **Red**: 40℉ ≤  second value
       - Indicates ambient temperature is low enough that tire grip at the contact patch is significantly reduced and traction loss is highly likely at most lean angles.
     * **Yellow**: 40℉ < second value ≤ 60℉ 
       - Indicates ambient temperature is low enough to increase the likelihood of traction loss at significant lean angles or abrupt emergency braking.
     * **Green**: 60℉ < second value ≤ 100℉
       - Indicates optimal ambient temperature to maintain tire grip at the contact patch.
     * **Red**: 100℉ < second value
       - Indicates ambient temperature has reached a level high enough to cause air expansion inside the tires, which may reduce the size of the contact patch. 

## Hardware Pin Configuration

| Function              | Nano Every Pin | Peripheral Pin        |
| --------------------- | -------------- | --------------------- |
| I²C SDA               | A4             | MLX90614 SDA, BME280 SDA |
| I²C SCL               | A5             | MLX90614 SCL, BME280 SCL |
| LCD SPI Clock         | D13 (SCK)      | Display CLK           |
| LCD SPI MOSI          | D11 (MOSI)     | Display DIN/MOSI      |
| LCD Chip Select       | D10            | Display CS            |
| LCD Data/Command      | D9             | Display DC            |
| LCD Reset             | D8             | Display RST           |

Power: 5 V tolerant variants.

## Logic 

Overview:

- `SensorChannel` — filtering, smoothing, and update-period management.
- `Display` — wraps an Adafruit ST77xx display over hardware SPI.
- `src/main.cpp` — Arduino `setup/loop`, real sensor drivers, and thresholds.
  - Smoothing constants and thresholds are configurable via `src/main.cpp` to fit
  different tire compounds and environments.

Firmware:

- Samples both sensors every 100 ms.
- Rejects samples outside configurable bounds.
- Applies exponential smoothing to promote accurate outputs to the display.
- Updates the tire temp line every 500 ms and the ambient temp line every 1000 ms.
- Draws two large text tiles on the LCD so the rider can quickly glance at the status.
- Color outputs that follow thresholds to enhance readability.

## Required Arduino Libraries

Required libraries:

- Adafruit MLX90614 
- Adafruit BME280
  - Depends on: (1) Adafruit Unified Sensor, (2) BusIO
- Adafruit GFX 
- Adafruit ST7735 
- Adafruit ST7789

## Build/Flash

1. Install **Arduino megaAVR** board package; select applicable board (e.g., _Arduino Nano Every_).
2. To compile via CLI:

   ```bash
   arduino-cli compile --fqbn arduino:megaavr:nona4809
   arduino-cli upload --fqbn arduino:megaavr:nona4809 -p /dev/ttyUSB0
   ```

3. Boot diagnostics observable via Serial Monitor @ 115200 baud.
    - Detection/initialization of sensors and display.

_The sketch initializes both sensors, draws the display once, and then_
_refreshes display each time one of the channels produces a new filtered value._

## Notes

- If either sensor fails to initialize, the firmware halts and prints an error
  over Serial so bad data doesn't reach the display.
- BME280 is probed at addresses `0x76` and `0x77`; adjustable via `src/main.cpp`
  for custom breakouts.

## License

This repository is publicly viewable for portfolio purposes only. The code is proprietary.
Copyright © 2025 Phaethon Order LLC. All rights reserved.
Contact [support@phaethon.llc](mailto:support@phaethon.llc) for licensing or reuse requests.

See [LICENSE](LICENSE)

## Contact
Questions or support: [support@phaethon.llc](mailto:support@phaethon.llc).
