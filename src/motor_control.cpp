#include "motor_control.h"
#include "config.h"
#include "rocket_state.h"
#include "logging.h"
#include <Arduino.h>

void initMotorControl() {
    // Configure motor control pins
    pinMode(PIN_MOTOR_DIRECTION, OUTPUT);
    pinMode(PIN_MOTOR_STOP, OUTPUT);
    pinMode(PIN_MOTOR_ENABLE, OUTPUT);
    
    // Configure PWM for speed control
    ledcSetup(0, MOTOR_PWM_FREQUENCY, MOTOR_PWM_RESOLUTION);
    ledcAttachPin(PIN_MOTOR_SPEED, 0);
    
    // Initialize to safe state
    digitalWrite(PIN_MOTOR_STOP, LOW);      // Not stopped
    digitalWrite(PIN_MOTOR_ENABLE, HIGH);   // Disabled initially
    digitalWrite(PIN_MOTOR_DIRECTION, HIGH); // Forward
    ledcWrite(0, 0);                        // Zero speed
    
    Logger.println("✅ Motor control initialized");
}

float calculateAcceleratedSpeed(float currentSpeed, float targetSpeed, float deltaTimeSeconds) {
    if (deltaTimeSeconds <= 0.0f) {
        return currentSpeed;
    }
    
    float difference = targetSpeed - currentSpeed;
    
    // If already at target, return it
    if (abs(difference) < 0.1f) {
        return targetSpeed;
    }
    
    // Calculate maximum change per second (percentage points)
    float maxChangePerSecond = MAX_ACCELERATION;
    
    // Calculate maximum change for this time period
    float maxChange = maxChangePerSecond * deltaTimeSeconds;
    
    // Apply logarithmic curve: change rate is proportional to remaining difference
    // This creates smooth acceleration/deceleration
    float changeRate = abs(difference) / 100.0f; // Normalize to 0-1
    float logarithmicFactor = log10(1.0f + changeRate * 9.0f); // Log curve: 0 to ~1
    float adjustedMaxChange = maxChange * (0.3f + 0.7f * logarithmicFactor); // Scale between 30-100% of max
    
    // Limit change to adjusted maximum
    float change = (difference > 0) ? 
        min(adjustedMaxChange, difference) : 
        max(-adjustedMaxChange, difference);
    
    return currentSpeed + change;
}

void updateMotorControl() {
    unsigned long currentTime = millis();
    float deltaTimeSeconds = (currentTime - rocketState.lastSpeedUpdate) / 1000.0f;
    
    // Update acceleration curve
    if (deltaTimeSeconds > 0.001f) { // Only if significant time has passed
        // Update current speed using acceleration curve
        if (isEnabled() && !isEmergencyStop()) {
            rocketState.currentSpeed = calculateAcceleratedSpeed(
                rocketState.currentSpeed, 
                rocketState.targetSpeed, 
                deltaTimeSeconds
            );
            
            // Update direction immediately (no acceleration curve for direction)
            rocketState.currentDirection = rocketState.targetDirection;
        } else {
            // System disabled or emergency stop - decelerate to zero
            rocketState.currentSpeed = calculateAcceleratedSpeed(
                rocketState.currentSpeed, 
                0.0f, 
                deltaTimeSeconds
            );
        }
        
        rocketState.lastSpeedUpdate = currentTime;
        
        // Update approximate velocity (simple integration)
        // This is a rough approximation - actual velocity would require more complex calculations
        float speedChange = rocketState.currentSpeed - (rocketState.approximateVelocity * 100.0f / MAX_MOTOR_SPEED);
        rocketState.approximateVelocity += speedChange * deltaTimeSeconds * 0.01f; // Scale factor
        rocketState.approximateVelocity = max(0.0f, rocketState.approximateVelocity); // Clamp to positive
    }
    
    // Apply motor control outputs
    if (isEmergencyStop() || !isEnabled()) {
        // Emergency stop or disabled
        digitalWrite(PIN_MOTOR_STOP, HIGH);     // Force stop
        digitalWrite(PIN_MOTOR_ENABLE, HIGH);   // Disable
        ledcWrite(0, 0);                        // Zero speed
    } else {
        // Normal operation
        digitalWrite(PIN_MOTOR_STOP, LOW);      // Not stopped
        digitalWrite(PIN_MOTOR_ENABLE, LOW);    // Enable motor controller
        
        // Set direction
        digitalWrite(PIN_MOTOR_DIRECTION, rocketState.currentDirection ? HIGH : LOW);
        
        // Set speed (0-255 PWM value from 0-100% speed)
        // Note: Motor controller expects 0-5V, ESP32 outputs 0-3.3V
        // May need voltage divider or level shifter - check motor controller specs
        uint8_t pwmValue = (uint8_t)((rocketState.currentSpeed / MAX_MOTOR_SPEED) * MOTOR_PWM_MAX_VALUE);
        ledcWrite(0, pwmValue);
    }
}

