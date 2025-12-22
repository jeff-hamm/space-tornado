#include "serial_interface.h"
#include "config.h"
#include "rocket_state.h"
#include "logging.h"
#include <Arduino.h>

static String serialBuffer = "";
static unsigned long lastSerialInput = 0;

void initSerialInterface() {
    Serial.begin(115200);
    Logger.addLogger(Serial);
    Logger.println("✅ Serial interface initialized");
    Logger.println("Commands: + (speed+10%), - (speed-10%), D (forward), R (reverse), F (fire), X (stop)");
}

void updateSerialInterface() {
    // Read available serial input
    while (Serial.available()) {
        char c = Serial.read();
        lastSerialInput = millis();
        
        // Process commands immediately (single character commands)
        switch (c) {
            case '+': {
                float newSpeed = getTargetSpeedPercent() + SPEED_INCREMENT;
                if (newSpeed > MAX_MOTOR_SPEED) newSpeed = MAX_MOTOR_SPEED;
                updateTargetSpeed(newSpeed);
                Logger.printf("📈 Speed increased to %.1f%%\n", newSpeed);
                break;
            }
            case '-': {
                float newSpeed = getTargetSpeedPercent() - SPEED_INCREMENT;
                if (newSpeed < 0.0f) newSpeed = 0.0f;
                updateTargetSpeed(newSpeed);
                Logger.printf("📉 Speed decreased to %.1f%%\n", newSpeed);
                break;
            }
            case 'R':
            case 'r': {
                // Toggle direction (reverse)
                updateTargetDirection(false);
                Logger.println("🔄 Direction set to REVERSE");
                break;
            }
            case 'D':
            case 'd': {
                // Set direction forward
                updateTargetDirection(true);
                Logger.println("🔄 Direction set to FORWARD");
                break;
            }
            case 'F':
            case 'f': {
                if (isEnabled() && !isEmergencyStop()) {
                    setFiringThrusters(true);
                    Logger.println("🔥 THRUSTERS FIRED!");
                    // Auto-stop after 2 seconds for safety (user can press again to continue)
                    // In a real scenario, you might want to require continuous 'F' presses
                } else {
                    Logger.println("⚠️ Cannot fire thrusters - system disabled or emergency stop active");
                }
                break;
            }
            case 'X':
            case 'x': {
                setEmergencyStop(true);
                Logger.println("🛑 EMERGENCY STOP ACTIVATED");
                break;
            }
            case '\n':
            case '\r':
                // Ignore newlines
                break;
            default:
                // Add to buffer for potential multi-character commands
                if (c >= 32 && c < 127) { // Printable ASCII
                    serialBuffer += c;
                }
                break;
        }
    }
    
    // Clear buffer if no input for a while
    if (serialBuffer.length() > 0 && (millis() - lastSerialInput) > SERIAL_COMMAND_TIMEOUT_MS) {
        serialBuffer = "";
    }
    
    // Periodic status output (every 2 seconds)
    static unsigned long lastStatusOutput = 0;
    if (millis() - lastStatusOutput > 2000) {
        Logger.printf("📊 Status - Speed: %.1f%%/%.1f%% (target/current), Dir: %s, Velocity: %.2f, Enabled: %s, Firing: %s\n",
            getTargetSpeedPercent(),
            getCurrentSpeedPercent(),
            getCurrentDirection() ? "FORWARD" : "REVERSE",
            getApproximateVelocity(),
            isEnabled() ? "YES" : "NO",
            isFiringThrusters() ? "YES" : "NO"
        );
        lastStatusOutput = millis();
    }
}

