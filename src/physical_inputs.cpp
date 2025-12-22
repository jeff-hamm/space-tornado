#include "physical_inputs.h"
#include "config.h"
#include "rocket_state.h"
#include "motor_control.h"
#include "logging.h"
#include <Arduino.h>

static unsigned long lastDirectionButtonPress = 0;
static unsigned long lastFireButtonPress = 0;
static bool lastDirectionButtonState = HIGH;
static bool lastFireButtonState = HIGH;
static bool lastEnableSwitchState = HIGH;

void initPhysicalInputs() {
    // Configure input pins
    pinMode(PIN_DIRECTION_BUTTON, INPUT_PULLUP);
    pinMode(PIN_FIRE_BUTTON, INPUT_PULLUP);
    pinMode(PIN_ENABLE_SWITCH, INPUT_PULLUP);
    // PIN_SPEED_POT is analog, no need to set mode
    
    // Read initial states
    lastDirectionButtonState = digitalRead(PIN_DIRECTION_BUTTON);
    lastFireButtonState = digitalRead(PIN_FIRE_BUTTON);
    lastEnableSwitchState = digitalRead(PIN_ENABLE_SWITCH);
    
    // Set initial enable state
    setEnabled(!lastEnableSwitchState); // Switch pulled up, LOW when on
    
    Logger.println("✅ Physical inputs initialized");
}

void updatePhysicalInputs() {
    unsigned long currentTime = millis();
    
    // Read enable switch (debounced)
    bool enableSwitchState = digitalRead(PIN_ENABLE_SWITCH);
    if (enableSwitchState != lastEnableSwitchState) {
        delay(PHYSICAL_INPUT_DEBOUNCE_MS);
        enableSwitchState = digitalRead(PIN_ENABLE_SWITCH);
        if (enableSwitchState != lastEnableSwitchState) {
            setEnabled(!enableSwitchState); // Switch pulled up, LOW when on
            lastEnableSwitchState = enableSwitchState;
        }
    }
    
    // Read speed potentiometer (only if enabled)
    if (isEnabled() && !isEmergencyStop()) {
        int potValue = analogRead(PIN_SPEED_POT);
        // Convert ADC reading (0-4095 for ESP32) to speed percentage (0-100)
        float speedPercent = (potValue / 4095.0f) * MAX_MOTOR_SPEED;
        updateTargetSpeed(speedPercent);
    }
    
    // Read direction button (debounced, edge-triggered)
    bool directionButtonState = digitalRead(PIN_DIRECTION_BUTTON);
    if (directionButtonState == LOW && lastDirectionButtonState == HIGH) {
        // Button pressed (edge detected)
        if (currentTime - lastDirectionButtonPress > PHYSICAL_INPUT_DEBOUNCE_MS) {
            // Toggle direction only if enabled
            if (isEnabled()) {
                updateTargetDirection(!getTargetDirection());
            }
            lastDirectionButtonPress = currentTime;
        }
    }
    lastDirectionButtonState = directionButtonState;
    
    // Read fire thrusters button (debounced, only if enabled)
    if (isEnabled() && !isEmergencyStop()) {
        bool fireButtonState = digitalRead(PIN_FIRE_BUTTON);
        if (fireButtonState == LOW && lastFireButtonState == HIGH) {
            // Button pressed (edge detected)
            if (currentTime - lastFireButtonPress > PHYSICAL_INPUT_DEBOUNCE_MS) {
                setFiringThrusters(true);
                lastFireButtonPress = currentTime;
            }
        } else if (fireButtonState == HIGH && lastFireButtonState == LOW) {
            // Button released
            if (currentTime - lastFireButtonPress > PHYSICAL_INPUT_DEBOUNCE_MS) {
                setFiringThrusters(false);
            }
        }
        lastFireButtonState = fireButtonState;
    } else {
        // System disabled or emergency stop - ensure thrusters are off
        if (isFiringThrusters()) {
            setFiringThrusters(false);
        }
        lastFireButtonState = digitalRead(PIN_FIRE_BUTTON);
    }
}

