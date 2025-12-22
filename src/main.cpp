#include <Arduino.h>
#include "config.h"
#include "logging.h"
#include "rocket_state.h"
#include "motor_control.h"
#include "physical_inputs.h"
#include "exhaust_control.h"
#include "wifi_manager.h"
#include "serial_interface.h"
#include "ble_interface.h"

void setup() {
    delay(1000); // Allow serial to initialize
    
    // Initialize logging first
    Logger.addLogger(Serial);
    Logger.println("\n\n=== Space Tornado Starting ===\n");
    
    // Initialize all systems
    initRocketState();
    initMotorControl();
    initPhysicalInputs();
    initExhaustControl();
    initSerialInterface();
    
    // Initialize BLE (NimBLE - for Web Bluetooth)
    initBLEInterface();
    
    // Initialize Bluetooth Classic (SPP - for serial terminal apps)
    initBluetoothClassic();
    
    // Initialize WiFi (non-blocking, will connect in background)
    initWiFi([]() {
        Logger.println("✅ WiFi connected - web interface available");
    });
    
    // Initialize OTA
    initOTA();
    
    Logger.println("✅ Space Tornado initialized and ready!");
}

void loop() {
    // Handle WiFi management (non-blocking)
    handleWiFiLoop();
    
    // Update physical inputs (potentiometer, buttons, switch)
    updatePhysicalInputs();
    
    // Update motor control (acceleration curve)
    updateMotorControl();
    
    // Update exhaust control
    updateExhaustControl();
    
    // Handle serial interface (terminal commands)
    updateSerialInterface();
    
    // Handle BLE interface (Web Bluetooth)
    updateBLEInterface();
    
    // Handle Bluetooth Classic interface (serial terminal apps)
    updateBluetoothClassic();
    
    // Small delay to prevent tight loop
    delay(10);
}
