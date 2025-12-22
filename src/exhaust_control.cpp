#include "exhaust_control.h"
#include "config.h"
#include "rocket_state.h"
#include "logging.h"
#include <Arduino.h>

void initExhaustControl() {
    pinMode(PIN_EXHAUST_SOLENOID, OUTPUT);
    pinMode(PIN_EXHAUST_IGNITER, OUTPUT);
    
    // Initialize to safe state (solenoid closed, igniter off)
    digitalWrite(PIN_EXHAUST_SOLENOID, LOW);
    digitalWrite(PIN_EXHAUST_IGNITER, LOW);
    
    Logger.println("✅ Exhaust control initialized");
}

void updateExhaustControl() {
    // Control exhaust system based on firing state
    if (isFiringThrusters() && isEnabled() && !isEmergencyStop()) {
        // Fire thrusters: open solenoid and activate igniter
        digitalWrite(PIN_EXHAUST_SOLENOID, HIGH);  // Open solenoid (SSR trigger HIGH)
        digitalWrite(PIN_EXHAUST_IGNITER, HIGH);   // Activate spark igniter
    } else {
        // Stop thrusters: close solenoid and deactivate igniter
        digitalWrite(PIN_EXHAUST_SOLENOID, LOW);   // Close solenoid
        digitalWrite(PIN_EXHAUST_IGNITER, LOW);    // Deactivate spark igniter
    }
}

