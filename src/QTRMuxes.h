// MinimalQTRMux.h
#ifndef QTRMUXES_H
#define QTRMUXES_H

#include <Arduino.h>
#include <CD74HC4067.h>

typedef enum {
    dualMux,
    singleMux
}QTRModes;
typedef enum {
    black,
    white,
    detect
}readModes;
class QTRMuxes {
private:
    uint8_t pS0, pS1, pS2, pS3, pSIG, pEN0, pEN1;
    uint16_t minVal[32];
    uint16_t maxVal[32];
    uint8_t _sensorCount = 0;
    uint16_t* _calibMin = nullptr;
    uint16_t* _calibMax = nullptr;

public:
    QTRMuxes(uint8_t S0, uint8_t S1, uint8_t S2, uint8_t S3,uint8_t SIG,uint8_t EN0, uint8_t EN1, uint8_t sensorCount);
    void begin(QTRModes mode);
    void readRaw(uint16_t* values,QTRModes mode);
    void readCalibrated(uint16_t* values,QTRModes mode);
    void calibrate(uint8_t samples,QTRModes mode);
    int readLine(uint16_t* values,readModes readMode,QTRModes mode);
    void readThroughSingleMux(uint16_t* values);
    void readThroughDualMux(uint16_t* values);
    uint16_t getCalibMin(uint8_t index);
    uint16_t getCalibMax(uint8_t index);


};

#endif
