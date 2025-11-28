#include "AS5600.h"

AS5600::AS5600(uint8_t sda, uint8_t scl, uint8_t address)
    : i2c_sda(sda), i2c_scl(scl), addr(address) {}

void AS5600::begin() {
    Wire.begin(i2c_sda, i2c_scl);
    Wire.setClock(400000);  // 400kHz for faster updates
    
    // Initialize timing
    lastUpdateTime = micros();
    
    // Initial read to set baseline
    Wire.beginTransmission(addr);
    Wire.write(REG_RAW_ANGLE_H);
    Wire.endTransmission(false);
    Wire.requestFrom(addr, (uint8_t)2);
    
    if (Wire.available() >= 2) {
        uint8_t high = Wire.read();
        uint8_t low = Wire.read();
        angleRaw = ((high << 8) | low) & 0x0FFF;
        prevAngleRaw = angleRaw;
        angleDeg = (angleRaw * 360.0f) / 4096.0f;
        angleFiltered = angleDeg;
        prevAngleDeg = angleDeg;
        kalman_angle = angleDeg;
    }
}

bool AS5600::update() {
    // Write register address
    Wire.beginTransmission(addr);
    Wire.write(REG_RAW_ANGLE_H);
    if (Wire.endTransmission(false) != 0) {  
        return false;  // Device not responding
    }

    // Read two bytes
    Wire.requestFrom(addr, (uint8_t)2);
    if (Wire.available() < 2)
        return false;

    uint8_t high = Wire.read();
    uint8_t low  = Wire.read();

    prevAngleRaw = angleRaw;
    angleRaw = ((high << 8) | low) & 0x0FFF;
    
    // Raw angle to degrees
    angleDeg = (angleRaw * 360.0f) / 4096.0f;
    
    // Calculate dt for Kalman filter
    uint32_t now = micros();
    float dt = (now - lastUpdateTime) / 1000000.0f;
    
    if (dt > 0.0001f && dt < 0.1f) {
        // Apply Kalman filter
        kalmanFilter(angleDeg, dt);
        angleFiltered = kalman_angle;
        
        // Compute velocity
        computeVelocity();
    }
    
    lastUpdateTime = now;
    return true;
}

void AS5600::kalmanFilter(float measurement, float dt) {
    // Handle wrap-around for angle measurement
    float diff = measurement - kalman_angle;
    if (diff > 180.0f) {
        measurement -= 360.0f;
    } else if (diff < -180.0f) {
        measurement += 360.0f;
    }
    
    // Prediction step
    // State prediction: angle(k) = angle(k-1) - bias
    // (no change expected, bias represents drift)
    kalman_angle = kalman_angle - kalman_bias * dt;
    
    // Covariance prediction: P = P + Q
    kalman_P[0][0] += Q_angle * dt;
    kalman_P[0][1] -= Q_bias * dt;
    kalman_P[1][0] -= Q_bias * dt;
    kalman_P[1][1] += Q_bias * dt;
    
    // Update step
    // Innovation: difference between measurement and prediction
    float innovation = measurement - kalman_angle;
    
    // Innovation covariance: S = P[0][0] + R
    float S = kalman_P[0][0] + R_measure;
    
    // Kalman gain: K = P / S
    float K[2];
    K[0] = kalman_P[0][0] / S;
    K[1] = kalman_P[1][0] / S;
    
    // State update
    kalman_angle += K[0] * innovation;
    kalman_bias += K[1] * innovation;
    
    // Wrap angle back to 0-360°
    if (kalman_angle >= 360.0f) kalman_angle -= 360.0f;
    if (kalman_angle < 0.0f) kalman_angle += 360.0f;
    
    // Covariance update: P = (I - K) * P
    float P00_temp = kalman_P[0][0];
    float P01_temp = kalman_P[0][1];
    
    kalman_P[0][0] -= K[0] * P00_temp;
    kalman_P[0][1] -= K[0] * P01_temp;
    kalman_P[1][0] -= K[1] * P00_temp;
    kalman_P[1][1] -= K[1] * P01_temp;
}

void AS5600::computeVelocity() {
    uint32_t now = micros();
    float dt = (now - lastUpdateTime) / 1000000.0f;
    
    if (dt > 0.0001f && dt < 0.1f) {
        // Use filtered angle for velocity calculation
        float angleChange = angleFiltered - prevAngleDeg;
        
        // Handle wrap-around
        if (angleChange > 180.0f) {
            angleChange -= 360.0f;
        } else if (angleChange < -180.0f) {
            angleChange += 360.0f;
        }
        
        // Calculate instantaneous velocity
        float instantVelocity = (angleChange * DEG_TO_RAD) / dt;
        
        // Moving average filter for velocity
        velocityWindow[velWindowIndex] = instantVelocity;
        velWindowIndex = (velWindowIndex + 1) % VEL_WINDOW_SIZE;
        
        // Calculate average
        float sum = 0;
        for (uint8_t i = 0; i < VEL_WINDOW_SIZE; i++) {
            sum += velocityWindow[i];
        }
        velocity = sum / VEL_WINDOW_SIZE;
    }
    
    prevAngleDeg = angleFiltered;
}