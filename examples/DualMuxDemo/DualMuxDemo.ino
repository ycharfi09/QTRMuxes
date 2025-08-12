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
    Serial.println("Calibration complete.");
}

void loop() {
    uint16_t sensorValues[SENSOR_COUNT];
    qtr.readCalibrated(sensorValues, dualMux);

    for (int i = 0; i < SENSOR_COUNT; i++) {
        Serial.print(sensorValues[i]);
        Serial.print('\t');
    }
    Serial.println();

    int pos = qtr.readLine(sensorValues, black,dualMux);
    Serial.print("Line position: ");
    Serial.println(pos);

    delay(100);
}
