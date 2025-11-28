#include <Arduino.h>
#include "AS5600.h"

AS5600 encoder;   // uses default pins SDA=21, SCL=22

unsigned long lastUpdate = 0;
const unsigned long UPDATE_INTERVAL = 20; // ms (50 Hz)

void setup() {
    Serial.begin(115200);
    encoder.begin();
    Serial.println("AS5600 non-blocking example started.");
}

void loop() {
    unsigned long now = millis();

    // Non-blocking periodic read
    if (now - lastUpdate >= UPDATE_INTERVAL) {
        lastUpdate = now;

        if (encoder.update()) {
            Serial.print("Raw: ");
            Serial.print(encoder.getRawAngle());
            Serial.print("   |   Deg: ");
            Serial.println(encoder.getAngleDegrees());
        }
    }

    // Your other logic (motor control, WiFi, etc.) continues without delay
}
