# QTRMuxes

[![arduino-library-badge](https://www.ardu-badge.com/badge/QTRMuxes.svg?)](https://www.ardu-badge.com/QTRMuxes)
[![PlatformIO Registry](https://badges.registry.platformio.org/packages/ycharfi09/library/QTRMuxes.svg)](https://registry.platformio.org/libraries/ycharfi09/QTRMuxes)


Lightweight Arduino library for reading Analog Pololu QTR sensors using one or two CD74HC4067 multiplexers.

Supports single and dual 16-channel mux setups (up to 32 sensors), sensor calibration, raw and calibrated readings, and line position calculation.

## Features

- Single or dual CD74HC4067 mux support
- Calibration routine with multiple samples
- Read raw sensor values
- Read calibrated sensor values (mapped 0–1000)
- Calculate line position from sensor readings
- Simple API similar to Pololu QTRSensors

## Dependencies

This library depends on the [CD74HC4067](https://github.com/waspinator/CD74HC4067) library by waspinator.  
Install it using the Arduino Library Manager or manually from the GitHub repository.

## Installation

1. Download or clone this repository.
2. Place the `QTRMuxes` folder in your Arduino `libraries` directory.
3. Make sure the CD74HC4067 library is installed.
4. Restart Arduino IDE.

## Usage

See the examples for usage with single or dual mux.

Basic example snippet:

```cpp
#include <QTRMuxes.h>

#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define SIG A0
#define EN0 6
#define EN1 7
#define SENSOR_COUNT 31

QTRMuxes qtr(S0, S1, S2, S3, SIG, EN0, EN1, SENSOR_COUNT);

void setup() {
  Serial.begin(115200);
  qtr.begin(dualMux);
  Serial.println("Calibrating sensors...");
  qtr.calibrate(20, dualMux);
  Serial.println("Calibration done.");
}

void loop() {
  uint16_t sensorValues[SENSOR_COUNT];
  qtr.readCalibrated(sensorValues, dualMux);

  for (int i = 0; i < SENSOR_COUNT; i++) {
    Serial.print(sensorValues[i]);
    Serial.print('\t');
  }
  Serial.println();

  int position = qtr.readLine(sensorValues, dualMux);
  Serial.print("Line position: ");
  Serial.println(position);

  delay(100);
}
