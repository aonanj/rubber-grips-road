# Contact Patch Temperature Sensor System (ConTemp)

> Embedded firmware for the Contact Patch Temperature Sensor System (ConTemp) motorcycle accessory. Targets Arduino Nano Every. This sketch drives:
> - A MLX90614 infrared (IR) sensor for front-tire surface temperature (I²C on A4/A5).
> - A BME280 for ambient temperature (shares the I²C bus).
> - An SPI color LCD (Adafruit ST77xx-compatible) showing color-coded status lines.

## Purpose
Provide a motorcycle operator with continuously updated information about tire surface temperature and lean angle safety. 

---

## Primary Components (Prototype)
1. **IR temperature sensor**
    * Directed to the front tire of the motorcycle in order to detect the surface temperature of the front tire. Suggested mounting locations: (1) radiator guard, (2) exterior surface of front-most outer fairing, (3) fork tube (if line-of-sight to tire surface), (4) exterior surface of cowl. 
    * Minimum effective characteristics:
      - D:S ≥ 12:1
      - FOV ≤ 5-10°
        - Collimating lens
      - Spectral range: 8-14 µm
      - Resolution ≤ 0.5°C 
      - Emissivity adjustable (0.8 - 1.0)
      - IP65 (or better)
3. **Ambient temperature sensor**
    * Preferably mounted at a location that minimizes the influence of engine and exhaust heat and wind on ambient temperature readings. Suggested mounting locations: (1) interior surface of an outer fairing, (2) underside of seat (if sufficient distance from exhaust system/muffler), (3) subframe proximate to rear suspension.  
3. **Microcontroller** 
    * Arduino Nano Every recommended for smaller form factor.
    * Preferably mounted on an interior surface of a fairing or behind the LCD display. 
4. **LCD display**
    * Preferably mounted on the handlebars, handlebar clamp, or steering stem nut. 
  
---

## Method of Operation

1. Outputs of the IR temperature sensor and ambient temperature sensor are inputs into the Arduino microcontroller. 
2. Microcontroller calculates and outputs values on a tire temp line and an ambient temp line to display on the LCD display. 
    * Tire temp line: carries values indicative of the temperature at the surface to which the IR temperature sensor is directed (i.e., front-tire surface temperature). 
    * Ambient temp line: carries values indicative of the ambient temperature of the motorcycle’s current operating environment. 
3. Microcontroller calculates primary display value corresponding to (approximate) tire surface temperature based on multiple samples received on the IR sensor ("tire temp") line.
    * Primary display value refreshed at 500ms intervals.
    * Primary display value derived based on:
       1. Passthrough filter applied to each sample received on IR temp sensor line. Samples not within a reasonable range are discarded.
       2. Samples are timestamped on arrival, discarded when stale or replaced by fresh sample.
       3. Samples within a window are assigned respective weights based on corresponding timestamps.
       4. Exponential smoothing algorithm using weighted samples within the window applied so primary display value is gradually updated as tire surface temp meaningfully changes.
4. Microcontroller calculates secondary display value corresponding to (approximate) ambient temperature based on multiple samples received on the ambient temp line.
    * Secondary display value refreshed at 1000ms intervals.
    * Secondary display value derived based on:
       1. Passthrough filter applied to each sample received on ambient temp line. Samples not within a reasonable range are discarded.
       2. Samples are timestamped on arrival, discarded when stale or replaced by fresh sample.
       3. Samples within a window are assigned respective weights based on corresponding timestamps.
       4. Exponential smoothing algorithm using weighted samples within the window applied so secondary display value is updated when ambient temp meaningfully changes.
5. Microcontroller staggers updates to primary display value and secondary display value. 
6. Microcontroller instructs the LCD display to display primary value corresponding to tire temp line in one of: 
    * **Red**: 90℉ ≤  first value
      - Indicates tire grip at the contact patch is too low to maintain traction during countersteering or emergency braking.
    * **Yellow**: 90℉ < first value ≤ 130℉ 
      - Indicates conservative countersteering may be utilized, but increased risk of traction loss at significant lean angles or abrupt emergency braking.
    * **Green**: 130℉ < first value ≤ 185℉
      - Indicates optimal surface temperature to maintain tire grip at the contact patch.
    * **Red**: 185℉ < first value
      - Indicates tire surface temperature at or near overheating state likely to cause reduced tire grip at the contact patch. 
7. Microcontroller instructs the LCD display to display secondary value corresponding to ambient temp line in one of: 
    * **Red**: 40℉ ≤  second value
      - Indicates ambient temperature is low enough to reduce tire grip at the contact patch in most conditions.
    * **Yellow**: 40℉ < second value ≤ 60℉ 
      - Indicates ambient temperature is low enough tire grip at the contact patch may be reduced in some conditions.
    * **Green**: 60℉ < second value ≤ 100℉
      - Indicates optimal ambient temperature to maintain tire grip at the contact patch.
    * **Red**: 100℉ < second value
      - Indicates ambient temperature is high enough to reduce tire grip at the contact patch in most conditions (high ambient temperature causes air expansion inside the tires). 

---

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

---

## Logic 

#### Overview:

- `SensorChannel` — filtering, smoothing, and update-period management.
- `Display` — wraps an Adafruit ST77xx display over hardware SPI.
- `src/main.cpp` — Arduino `setup/loop`, real sensor drivers, and thresholds.
  - Smoothing constants and thresholds are configurable via `src/main.cpp` to fit
  different tire compounds and environments.

#### Firmware:

- Samples both sensors every 100 ms.
- Rejects samples outside configurable bounds.
- Applies exponential smoothing to improve accuracy of displayed values. 
- Updates the tire temp line every 500 ms and the ambient temp line every 1000 ms.
- Draws two visually distinct text tiles on the LCD so the rider can quickly glance at the status:
   - Large (primary) text tile displays data from tire temp line. Displayed above static "Tire Temp" label.
   - Smaller (50%) text tile displays data from ambient temp line (ambient temp intended only as a supplement to tire temp). Displayed above static "Ambient" label.
- Color outputs that follow thresholds to enhance readability.

---

## Implementation

#### Required Arduino Libraries

- Adafruit MLX90614 
- Adafruit BME280
  - Depends on: (1) Adafruit Unified Sensor, (2) BusIO
- Adafruit GFX 
- Adafruit ST7735 
- Adafruit ST7789

#### Build/Flash

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

---

## Notes

- If either sensor fails to initialize, the firmware halts and prints an error
  over Serial so bad data doesn't reach the display.
- BME280 is probed at addresses `0x76` and `0x77`; adjustable via `src/main.cpp`
  for custom breakouts.

---

## V2.0+ Features (planned)

1. User-configurable thresholds
2. Single indicator for estimated tire grip, aggregates and weights samples from both input lines. 
3. Quality control/accuracy checks for IR temperature sensor so primary display value avoids presenting stale or inaccurate information.

## License

This repository is publicly viewable for portfolio purposes only. The code is proprietary.
Copyright © 2025 Phaethon Order LLC. All rights reserved.
Contact [support@phaethon.llc](mailto:support@phaethon.llc) for licensing or reuse requests.

*See* [LICENSE](LICENSE.md) for terms.

---

## Contact
Questions or support: [support@phaethon.llc](mailto:support@phaethon.llc).
