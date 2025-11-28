#pragma once
#include <Arduino.h>
#include <Wire.h>

class AS5600 {
public:
    AS5600(uint8_t sda = 21, uint8_t scl = 22, uint8_t address = 0x36);

    void begin();
    bool update();   // non-blocking refresh
    
    uint16_t getRawAngle() const { return angleRaw; }
    float getAngleDegrees() const { return angleDeg; }
    float getAngleRadians() const { return angleDeg * DEG_TO_RAD; }
    
    // Kalman filtered angle
    float getFilteredAngleDegrees() const { return angleFiltered; }
    float getFilteredAngleRadians() const { return angleFiltered * DEG_TO_RAD; }
    
    // Velocity in rad/s
    float getVelocity() const { return velocity; }

private:
    uint8_t i2c_sda;
    uint8_t i2c_scl;
    uint8_t addr;

    uint16_t angleRaw = 0;
    uint16_t prevAngleRaw = 0;
    float angleDeg = 0;
    float angleFiltered = 0;  // Kalman filtered angle
    
    float velocity = 0;
    float prevAngleDeg = 0;
    uint32_t lastUpdateTime = 0;
    
    // Kalman filter state
    float kalman_angle = 0;
    float kalman_bias = 0;
    float kalman_P[2][2] = {{1, 0}, {0, 1}};
    
    // Moving average for velocity
    static const uint8_t VEL_WINDOW_SIZE = 5;
    float velocityWindow[VEL_WINDOW_SIZE] = {0};
    uint8_t velWindowIndex = 0;

    static const uint8_t REG_RAW_ANGLE_H = 0x0C;
    static const uint8_t REG_RAW_ANGLE_L = 0x0D;
    static constexpr float DEG_TO_RAD = 3.1 / 180.0f;
    
    // Kalman filter parameters (tune these for your application)
    static constexpr float Q_angle = 0.001f;   // Process noise - angle (lower = trust model more)
    static constexpr float Q_bias = 0.003f;    // Process noise - bias
    static constexpr float R_measure = 0.03f;  // Measurement noise (lower = trust sensor more)
    
    void computeVelocity();
    void kalmanFilter(float measurement, float dt);
};