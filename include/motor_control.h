#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H

#include "config.h"
#include "rocket_state.h"

// Initialize motor control system
void initMotorControl();

// Update motor control (call this regularly in loop)
void updateMotorControl();

// Apply logarithmic acceleration curve
float calculateAcceleratedSpeed(float currentSpeed, float targetSpeed, float deltaTimeSeconds);

#endif // MOTOR_CONTROL_H

