#include "QTRMuxes.h"
#include <Arduino.h>
#include <CD74HC4067.h>

QTRMuxes::QTRMuxes(uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3, uint8_t SIG, uint8_t EN0, uint8_t EN1,uint8_t sensorCount) {
    pS0 = S0;
    pS1 = S1;
    pS2 = S2;
    pS3 = S3;
    pSIG = SIG;
    pEN0 = EN0;
    pEN1 = EN1;
    _sensorCount = sensorCount;
}

void QTRMuxes::begin(QTRModes mode) {
    if (!_calibMin) {
        _calibMin = new uint16_t[_sensorCount];
        _calibMax = new uint16_t[_sensorCount];
        for (uint8_t i = 0; i < _sensorCount; i++) {
            _calibMin[i] = 1023;
            _calibMax[i] = 0;
        }
    }
    pinMode(pSIG, INPUT);
    if (mode == dualMux) {
        pinMode(pEN0, OUTPUT);
        pinMode(pEN1, OUTPUT);
    }
}

void QTRMuxes::readThroughSingleMux( uint16_t *sensorValues) {
    CD74HC4067 mux(pS0,pS1, pS2, pS3);
    for (uint8_t i = 0; i < _sensorCount; i++) {
        mux.channel(i);
        delayMicroseconds(10);
        sensorValues[i] = analogRead(pSIG);
    }
}

void QTRMuxes::readThroughDualMux( uint16_t *sensorValues) {
    begin(dualMux);
    CD74HC4067 mux(pS0, pS1, pS2, pS3);
    for (uint8_t i = 0; i < _sensorCount; i++) {
        if (i < 16) {
            digitalWrite(pEN0, LOW);
            digitalWrite(pEN1, HIGH);
            mux.channel(i);
        } else {
            digitalWrite(pEN0, HIGH);
            digitalWrite(pEN1, LOW);
            mux.channel(i - 16);
        }
        delayMicroseconds(10);
        sensorValues[i] = analogRead(pSIG);
    }
}
void QTRMuxes::readCalibrated(uint16_t *sensorValues,QTRModes mode) {
    if (mode == dualMux) {
        readThroughDualMux(sensorValues);
        for (uint8_t i = 0; i < _sensorCount; i++) {
            uint16_t value = sensorValues[i];
            uint16_t minVal = _calibMin[i];
            uint16_t maxVal = _calibMax[i];
            if (maxVal != minVal) {
                value = constrain(value, minVal, maxVal);
                sensorValues[i] = map(value, minVal, maxVal, 0, 1000);
            } else {
                sensorValues[i] = 0;
            }
        }}
    else {
        readThroughSingleMux(sensorValues);
        for (uint8_t i = 0; i < _sensorCount; i++) {
            uint16_t value = sensorValues[i];
            uint16_t minVal = _calibMin[i];
            uint16_t maxVal = _calibMax[i];
            if (maxVal != minVal) {
                value = constrain(value, minVal, maxVal);
                sensorValues[i] = map(value, minVal, maxVal, 0, 1000);
            } else {
                sensorValues[i] = 0;
            }
        }}
    }


void QTRMuxes::calibrate(uint8_t samples,QTRModes mode) {
  if (mode == dualMux){
        for (uint8_t i = 0; i < _sensorCount; i++) {
        _calibMin[i] = 1023;
        _calibMax[i] = 0;
    }
        uint16_t temp[_sensorCount];
        for (uint8_t s = 0; s < samples; s++) {
            readThroughDualMux(temp);
            for (uint8_t i = 0; i < _sensorCount; i++) {
                if (temp[i] < _calibMin[i]) _calibMin[i] = temp[i];
                if (temp[i] > _calibMax[i]) _calibMax[i] = temp[i];
            }
        }}
    else {
        for (uint8_t i = 0; i < _sensorCount; i++) {
            _calibMin[i] = 1023;
            _calibMax[i] = 0;
        }
        uint16_t temp[_sensorCount];
        for (uint8_t s = 0; s < samples; s++) {
            readThroughSingleMux(temp);
            for (uint8_t i = 0; i < _sensorCount; i++) {
                if (temp[i] < _calibMin[i]) _calibMin[i] = temp[i];
                if (temp[i] > _calibMax[i]) _calibMax[i] = temp[i];
            }
        }
    }
}

int QTRMuxes::readLine(uint16_t *sensorValues,readModes readMode,QTRModes mode) {\
    if (readMode==black) {
        readCalibrated(sensorValues, mode);
        uint32_t avg = 0;
        uint16_t sum = 0;
        for (uint8_t i = 0; i < _sensorCount; i++) {
            uint16_t val = sensorValues[i];
            if (val > 50) {
                avg += (uint32_t)val * i * 1000;
                sum += val;
            }
        }return (sum == 0) ? -1 : avg / sum;
    }if (readMode==white) {
        readCalibrated(sensorValues, mode);
        uint32_t avg = 0;
        uint16_t sum = 0;
        for (uint8_t i = 0; i < _sensorCount; i++) {
            uint16_t val = 1000 - sensorValues[i];
            if (val > 50) {
                avg += (uint32_t)val * i * 1000;
                sum += val;
            }
        }return (sum == 0) ? -1 : avg / sum;
    }else {
        readCalibrated(sensorValues, mode);

        // Step 1: Compute average sensor value
        uint32_t total = 0;
        for (uint8_t i = 0; i < _sensorCount; i++) {
            total += sensorValues[i];
        }
        uint16_t avgBrightness = total / _sensorCount;

        // Step 2: Auto-detect line color
        bool whiteLine = (avgBrightness > 500); // >500 = mostly black background, white line

        // Step 3: Line position calculation
        uint32_t avg = 0;
        uint16_t sum = 0;
        for (uint8_t i = 0; i < _sensorCount; i++) {
            uint16_t val = whiteLine ? (1000 - sensorValues[i]) : sensorValues[i];
            if (val > 50) {
                avg += (uint32_t)val * i * 1000;
                sum += val;
            }
        }

        return (sum == 0) ? -1 : avg / sum;
    }
    }

uint16_t QTRMuxes::getCalibMin(uint8_t index) {
    return _calibMin[index];
}

uint16_t QTRMuxes::getCalibMax(uint8_t index) {
    return _calibMax[index];
}



