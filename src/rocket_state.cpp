#include "rocket_state.h"
#include "logging.h"

RocketState rocketState;

void initRocketState() {
    rocketState.targetSpeed = 0.0f;
    rocketState.currentSpeed = 0.0f;
    rocketState.targetDirection = true;
    rocketState.currentDirection = true;
    rocketState.enabled = false;
    rocketState.firingThrusters = false;
    rocketState.emergencyStop = false;
    rocketState.lastSpeedUpdate = millis();
    rocketState.approximateVelocity = 0.0f;
    
    Logger.println("✅ Rocket state initialized");
}

void updateTargetSpeed(float speed) {
    if (speed < 0.0f) speed = 0.0f;
    if (speed > MAX_MOTOR_SPEED) speed = MAX_MOTOR_SPEED;
    
    rocketState.targetSpeed = speed;
    Logger.printf("🎯 Target speed set to: %.1f%%\n", speed);
}

void updateTargetDirection(bool forward) {
    rocketState.targetDirection = forward;
    Logger.printf("🎯 Target direction set to: %s\n", forward ? "FORWARD" : "REVERSE");
}

void setEmergencyStop(bool stop) {
    rocketState.emergencyStop = stop;
    if (stop) {
        rocketState.targetSpeed = 0.0f;
        Logger.println("🛑 EMERGENCY STOP ACTIVATED");
    } else {
        Logger.println("✅ Emergency stop cleared");
    }
}

void setEnabled(bool enabled) {
    rocketState.enabled = enabled;
    if (!enabled) {
        // When disabled, set target speed to 0
        rocketState.targetSpeed = 0.0f;
        Logger.println("🔒 System disabled");
    } else {
        Logger.println("🔓 System enabled");
    }
}

void setFiringThrusters(bool firing) {
    rocketState.firingThrusters = firing;
    if (firing) {
        Logger.println("🔥 THRUSTERS FIRING!");
    } else {
        Logger.println("💨 Thrusters stopped");
    }
}

float getCurrentSpeedPercent() {
    return rocketState.currentSpeed;
}

float getTargetSpeedPercent() {
    return rocketState.targetSpeed;
}

bool getCurrentDirection() {
    return rocketState.currentDirection;
}

bool getTargetDirection() {
    return rocketState.targetDirection;
}

bool isEnabled() {
    return rocketState.enabled && !rocketState.emergencyStop;
}

bool isFiringThrusters() {
    return rocketState.firingThrusters;
}

bool isEmergencyStop() {
    return rocketState.emergencyStop;
}

float getApproximateVelocity() {
    return rocketState.approximateVelocity;
}

