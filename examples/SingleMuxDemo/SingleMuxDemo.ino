#include <QTRMuxes.h>

#define S0 2
#define S1 3
#define S2 4
#define S3 5
#define SIG A0
#define SENSOR_COUNT 16

QTRMuxes qtr(S0, S1, S2, S3, SIG, 255, 255, SENSOR_COUNT); // EN0 and EN1 unused (255)

void setup() {
    Serial.begin(115200);
    qtr.begin(singleMux);

    Serial.println("Calibrating sensors...");
    qtr.calibrate(20, singleMux);
    Serial.println("Calibration complete.");
}

void loop() {
    uint16_t sensorValues[SENSOR_COUNT];
    qtr.readCalibrated(sensorValues, singleMux);

    for (int i = 0; i < SENSOR_COUNT; i++) {
        Serial.print(sensorValues[i]);
        Serial.print('\t');
    }
    Serial.println();

    int pos = qtr.readLine(sensorValues, black,singleMux);
    Serial.print("Line position: ");
    Serial.println(pos);

    delay(100);
}
