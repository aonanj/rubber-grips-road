# Tire Temperature Monitor (C++)

_embedded-style logic for a motorcycle front tire infrared sensor, ambient temperature sensor, and color LCD display_

## Overview

This project models the firmware logic that would execute on a microcontroller responsible for:

- Reading a front-tire infrared temperature sensor every 100 ms.
- Reading an ambient temperature sensor on the same cadence.
- Filtering samples outside reasonable thresholds before they are ever used.
- Applying exponential smoothing to retain a memory of the past while still reacting to new data.
- Emitting an updated tire surface temperature every 0.5 s and an updated ambient temperature every 1.0 s.
- Driving a color LCD with the status ranges specified in the requirements.

To keep the example buildable on a desktop, lightweight pseudo-sensor classes (`InfraredSensor`, `AmbientSensor`) produce noisy measurements that exercise the filtering and smoothing pipeline.

## Building

```bash
cmake -S . -B build
cmake --build build
```

This produces the executable `build/tire_monitor`.

## Running

```bash
./build/tire_monitor
```

The program runs indefinitely (as the real microcontroller firmware would) and prints the current LCD state to the terminal. Press `Ctrl+C` to stop it.

## Architecture Highlights

- `SensorChannel` encapsulates threshold filtering, exponential smoothing, and update-period management.
- `Display` translates the logic table in the prompt into color-coded text output for each channel.
- `SimulationSensors` offer deterministic, low-noise inputs during development; replace these with real drivers when integrating with hardware.

Key thresholds and smoothing weights are centralized in `main.cpp` for easy tuning per platform.
