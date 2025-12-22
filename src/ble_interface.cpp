#include "ble_interface.h"
#include "config.h"
#include "rocket_state.h"
#include "logging.h"

// ============================================================================
// TRUE BLE (Bluetooth Low Energy) IMPLEMENTATION
// Uses NimBLE for efficient BLE server - works with Web Bluetooth API
// ============================================================================

#include <NimBLEDevice.h>

static NimBLEServer* pServer = nullptr;
static NimBLECharacteristic* pCommandChar = nullptr;
static NimBLECharacteristic* pStatusChar = nullptr;
static bool bleDeviceConnected = false;
static bool oldBleDeviceConnected = false;
static unsigned long lastStatusNotify = 0;

// Process a single command character or command string
void processBLECommand(const std::string& cmd) {
    if (cmd.empty()) return;
    
    char c = cmd[0];
    
    switch (c) {
        case '+': {
            float newSpeed = getTargetSpeedPercent() + SPEED_INCREMENT;
            if (newSpeed > MAX_MOTOR_SPEED) newSpeed = MAX_MOTOR_SPEED;
            updateTargetSpeed(newSpeed);
            Logger.printf("BLE: Speed +10%% → %.1f%%\n", newSpeed);
            break;
        }
        case '-': {
            float newSpeed = getTargetSpeedPercent() - SPEED_INCREMENT;
            if (newSpeed < 0.0f) newSpeed = 0.0f;
            updateTargetSpeed(newSpeed);
            Logger.printf("BLE: Speed -10%% → %.1f%%\n", newSpeed);
            break;
        }
        case 'S': case 's': {
            // Speed command: S50 = set speed to 50%
            if (cmd.length() > 1) {
                float speed = atof(cmd.c_str() + 1);
                speed = constrain(speed, 0.0f, MAX_MOTOR_SPEED);
                updateTargetSpeed(speed);
                Logger.printf("BLE: Speed set to %.1f%%\n", speed);
            }
            break;
        }
        case 'R': case 'r': {
            updateTargetDirection(false);
            Logger.println("BLE: Direction → REVERSE");
            break;
        }
        case 'D': case 'd': {
            updateTargetDirection(true);
            Logger.println("BLE: Direction → FORWARD");
            break;
        }
        case 'F': {
            if (isEnabled() && !isEmergencyStop()) {
                setFiringThrusters(true);
                Logger.println("BLE: 🔥 THRUSTERS FIRING!");
            }
            break;
        }
        case 'f': {
            setFiringThrusters(false);
            Logger.println("BLE: Thrusters stopped");
            break;
        }
        case 'X': case 'x': {
            setEmergencyStop(true);
            Logger.println("BLE: 🛑 EMERGENCY STOP!");
            break;
        }
        case 'C': case 'c': {
            // Clear emergency stop
            setEmergencyStop(false);
            Logger.println("BLE: Emergency stop cleared");
            break;
        }
        case '?': {
            // Status query - will be sent via notification
            Logger.println("BLE: Status requested");
            break;
        }
        default:
            break;
    }
}

// BLE Server callbacks
class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer* pServer) {
        bleDeviceConnected = true;
        Logger.println("📱 BLE client connected");
    }

    void onDisconnect(NimBLEServer* pServer) {
        bleDeviceConnected = false;
        Logger.println("📱 BLE client disconnected");
        // Restart advertising
        NimBLEDevice::startAdvertising();
    }
};

// Command characteristic callbacks
class CommandCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic* pCharacteristic) {
        std::string value = pCharacteristic->getValue();
        if (value.length() > 0) {
            processBLECommand(value);
        }
    }
};

void initBLEInterface() {
    Logger.println("🔵 Initializing BLE (NimBLE)...");
    
    // Initialize NimBLE
    NimBLEDevice::init(BLE_DEVICE_NAME);
    
    // Set power level for better range
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);
    
    // Create server
    pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());
    
    // Create service
    NimBLEService* pService = pServer->createService(BLE_SERVICE_UUID);
    
    // Create command characteristic (write)
    pCommandChar = pService->createCharacteristic(
        BLE_COMMAND_CHAR_UUID,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR
    );
    pCommandChar->setCallbacks(new CommandCallbacks());
    
    // Create status characteristic (read + notify)
    pStatusChar = pService->createCharacteristic(
        BLE_STATUS_CHAR_UUID,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY
    );
    
    // Start service
    pService->start();
    
    // Start advertising
    NimBLEAdvertising* pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);
    pAdvertising->setMaxPreferred(0x12);
    NimBLEDevice::startAdvertising();
    
    Logger.printf("✅ BLE initialized as '%s'\n", BLE_DEVICE_NAME);
    Logger.println("   Service UUID: " BLE_SERVICE_UUID);
}

void updateBLEInterface() {
    // Handle connection state changes
    if (bleDeviceConnected && !oldBleDeviceConnected) {
        // Just connected - send initial status
        lastStatusNotify = 0;
    }
    
    if (!bleDeviceConnected && oldBleDeviceConnected) {
        // Just disconnected
    }
    
    oldBleDeviceConnected = bleDeviceConnected;
    
    // Send status notifications periodically
    if (bleDeviceConnected && pStatusChar) {
        unsigned long now = millis();
        if (now - lastStatusNotify >= BLE_STATUS_NOTIFY_INTERVAL_MS) {
            // Format: "S:50.0,T:60.0,D:1,E:1,F:0"
            char status[64];
            snprintf(status, sizeof(status), 
                "S:%.1f,T:%.1f,D:%d,E:%d,F:%d",
                getCurrentSpeedPercent(),
                getTargetSpeedPercent(),
                getCurrentDirection() ? 1 : 0,
                isEnabled() ? 1 : 0,
                isFiringThrusters() ? 1 : 0
            );
            
            pStatusChar->setValue(status);
            pStatusChar->notify();
            
            lastStatusNotify = now;
        }
    }
}


// ============================================================================
// BLUETOOTH CLASSIC (SPP) IMPLEMENTATION
// For compatibility with serial Bluetooth terminal apps
// ============================================================================

#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#warning "Bluetooth Classic may not be fully enabled"
#endif

static BluetoothSerial SerialBT;
static bool btClassicInitialized = false;

void initBluetoothClassic() {
    Logger.println("🔷 Initializing Bluetooth Classic (SPP)...");
    
    if (!SerialBT.begin(BT_CLASSIC_DEVICE_NAME)) {
        Logger.println("❌ Bluetooth Classic initialization failed!");
        return;
    }
    
    btClassicInitialized = true;
    Logger.printf("✅ Bluetooth Classic initialized as '%s'\n", BT_CLASSIC_DEVICE_NAME);
    Logger.println("   Commands: +, -, D, R, F, X, ? (status)");
}

void updateBluetoothClassic() {
    if (!btClassicInitialized) return;
    
    // Read available BT Classic input
    while (SerialBT.available()) {
        char c = SerialBT.read();
        
        switch (c) {
            case '+': {
                float newSpeed = getTargetSpeedPercent() + SPEED_INCREMENT;
                if (newSpeed > MAX_MOTOR_SPEED) newSpeed = MAX_MOTOR_SPEED;
                updateTargetSpeed(newSpeed);
                SerialBT.printf("Speed → %.1f%%\n", newSpeed);
                break;
            }
            case '-': {
                float newSpeed = getTargetSpeedPercent() - SPEED_INCREMENT;
                if (newSpeed < 0.0f) newSpeed = 0.0f;
                updateTargetSpeed(newSpeed);
                SerialBT.printf("Speed → %.1f%%\n", newSpeed);
                break;
            }
            case 'R': case 'r': {
                updateTargetDirection(false);
                SerialBT.println("Direction → REVERSE");
                break;
            }
            case 'D': case 'd': {
                updateTargetDirection(true);
                SerialBT.println("Direction → FORWARD");
                break;
            }
            case 'F': case 'f': {
                if (isEnabled() && !isEmergencyStop()) {
                    setFiringThrusters(true);
                    SerialBT.println("🔥 THRUSTERS FIRING!");
                } else {
                    SerialBT.println("Cannot fire - system disabled");
                }
                break;
            }
            case 'X': case 'x': {
                setEmergencyStop(true);
                SerialBT.println("🛑 EMERGENCY STOP!");
                break;
            }
            case '?': {
                SerialBT.printf("Speed: %.1f%%/%.1f%%, Dir: %s, En: %s, Fire: %s\n",
                    getTargetSpeedPercent(),
                    getCurrentSpeedPercent(),
                    getCurrentDirection() ? "FWD" : "REV",
                    isEnabled() ? "YES" : "NO",
                    isFiringThrusters() ? "YES" : "NO"
                );
                break;
            }
            default:
                break;
        }
    }
    
    // Periodic status (every 5 seconds)
    static unsigned long lastBTStatus = 0;
    if (millis() - lastBTStatus > 5000) {
        if (SerialBT.hasClient()) {
            SerialBT.printf("[%lus] Speed: %.1f%%, Dir: %s, En: %s\n",
                millis() / 1000,
                getCurrentSpeedPercent(),
                getCurrentDirection() ? "FWD" : "REV",
                isEnabled() ? "YES" : "NO"
            );
        }
        lastBTStatus = millis();
    }
}

