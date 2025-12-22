#pragma once

// Pin assignments - adjust as needed
#define PIN_MOTOR_SPEED 25          // PWM output for motor speed (0-5V via voltage divider)
#define PIN_MOTOR_DIRECTION 26      // Direction control (LOW = Reverse, HIGH = Forward)
#define PIN_MOTOR_STOP 27           // Stop control (HIGH = Force Stop)
#define PIN_MOTOR_ENABLE 14         // Enable control (LOW = Enabled)

#define PIN_EXHAUST_SOLENOID 33     // SSR trigger for exhaust solenoid
#define PIN_EXHAUST_IGNITER 32      // Spark igniter control (if separate)

#define PIN_SPEED_POT 34            // Analog input for speed potentiometer
#define PIN_DIRECTION_BUTTON 35     // Direction button (with pull-up)
#define PIN_FIRE_BUTTON 39          // Fire thrusters button (with pull-up)
#define PIN_ENABLE_SWITCH 36        // Enable switch (with pull-up)

// Configuration constants
#define MAX_MOTOR_SPEED 100.0f      // Maximum motor speed percentage (0-100)
#define MAX_ACCELERATION 5.0f       // Maximum acceleration per second (percentage points)
#define SPEED_INCREMENT 10.0f       // Speed increment/decrement percentage (for terminal commands)
#define MOTOR_PWM_FREQUENCY 5000    // PWM frequency for motor speed control (Hz)
#define MOTOR_PWM_RESOLUTION 8      // PWM resolution in bits (0-255)
#define MOTOR_PWM_MAX_VALUE 255     // Maximum PWM value

// Timing constants
#define ACCELERATION_UPDATE_MS 50   // Update acceleration every 50ms
#define PHYSICAL_INPUT_DEBOUNCE_MS 50  // Debounce time for buttons/switches
#define SERIAL_COMMAND_TIMEOUT_MS 1000 // Timeout for serial command processing

// Web server
#define WEB_SERVER_PORT 80

// Bluetooth Classic (SPP)
#define BT_CLASSIC_DEVICE_NAME "SpaceTornado-SPP"

// BLE Configuration
#define BLE_DEVICE_NAME "SpaceTornado"
#define BLE_SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BLE_COMMAND_CHAR_UUID   "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define BLE_STATUS_CHAR_UUID    "beb5483f-36e1-4688-b7f5-ea07361b26a9"
#define BLE_STATUS_NOTIFY_INTERVAL_MS 500

