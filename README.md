# Rubber Grips Road (Arduino Nano Every)

Embedded firmware for the Rubber Grips Road accessory. The sketch targets the
Arduino Nano Every and drives:

- A MLX90614 infrared sensor for front-tire surface temperature (I²C on A4/A5).
- A BME280 for ambient temperature (shares the I²C bus).
- An SPI color LCD (Adafruit ST77xx-compatible) showing color-coded status lines.

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

## Firmware Behavior

- Samples both sensors every 100 ms.
- Rejects samples outside configurable bounds before they ever affect the UI.
- Applies exponential smoothing to preserve short-term history.
- Updates the tire line every 500 ms and the ambient line every 1000 ms.
- Colors follow the same thresholds used in the desktop prototype.
- Draws two large text tiles on the LCD so the rider can glance at the status.

Key logic lives in:

- `SensorChannel` — filtering, smoothing, and update-period management.
- `Display` — wraps an Adafruit ST77xx display over hardware SPI.
- `src/main.cpp` — Arduino `setup/loop`, real sensor drivers, and thresholds.

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
- The BME280 is probed at addresses `0x76` and `0x77`; adjust in `src/main.cpp`
  if your breakout uses something custom.
- Adjust the smoothing constants or thresholds directly in `src/main.cpp` to fit
  your tire compound or environmental testing.
