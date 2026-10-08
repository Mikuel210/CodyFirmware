#include <Wire.h>
#include <TCS_Clone.h>

// Default: address 0x29, ~103ms integration, 1x gain
TCS_Clone sensor;

void setup() {
    Serial.begin(115200);
    delay(500);

    if (!sensor.begin()) {
        Serial.printf("Sensor not found (chip ID: 0x%02X). Check wiring.\n",
                      sensor.chipID());
        while (1);
    }

    Serial.printf("Sensor ready (chip ID: 0x%02X)\n", sensor.chipID());

    // Optional: crank up sensitivity for dim environments
    // sensor.setGain(TCS_GAIN_16X);
    // sensor.setIntegrationTime(TCS_ATIME_700MS);
}

void loop() {
    RGBC c = sensor.read();

    if (!c.valid) {
        Serial.println("Read timed out");
        return;
    }

    Serial.printf("R: %5d  G: %5d  B: %5d  C: %5d  | %5d K  %4d lux\n",
                  c.r, c.g, c.b, c.c, c.colorTemp, c.lux);

    delay(500);
}
