#ifndef ROCKET_STATE_H
#define ROCKET_STATE_H

#include "config.h"

struct RocketState {
    // Target values (set by inputs)
    float targetSpeed;          // 0-100% target speed
    bool targetDirection;       // true = forward, false = reverse
    
    // Current values (applied to motor, changed by acceleration curve)
    float currentSpeed;         // 0-100% current speed
    bool currentDirection;      // true = forward, false = reverse
    
    // System state
    bool enabled;               // System enabled (enable switch)
    bool firingThrusters;       // Thrusters are firing
    bool emergencyStop;         // Emergency stop active
    
    // Timing/velocity tracking
    unsigned long lastSpeedUpdate;  // Last time speed was updated
    float approximateVelocity;      // Approximate velocity (relative units)
    
    RocketState() : 
        targetSpeed(0.0f),
        targetDirection(true),
        currentSpeed(0.0f),
        currentDirection(true),
        enabled(false),
        firingThrusters(false),
        emergencyStop(false),
        lastSpeedUpdate(0),
        approximateVelocity(0.0f)
    {}
};

// Global state instance
extern RocketState rocketState;

// State management functions
void initRocketState();
void updateTargetSpeed(float speed);
void updateTargetDirection(bool forward);
void setEmergencyStop(bool stop);
void setEnabled(bool enabled);
void setFiringThrusters(bool firing);

// Get state info
float getCurrentSpeedPercent();
float getTargetSpeedPercent();
bool getCurrentDirection();
bool getTargetDirection();
bool isEnabled();
bool isFiringThrusters();
bool isEmergencyStop();
float getApproximateVelocity();

#endif // ROCKET_STATE_H

