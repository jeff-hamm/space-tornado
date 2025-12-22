#ifndef BLE_INTERFACE_H
#define BLE_INTERFACE_H

#include "config.h"

// True BLE (Bluetooth Low Energy) interface using NimBLE
void initBLEInterface();
void updateBLEInterface();

// Bluetooth Classic SPP interface (legacy, for serial terminal apps)
void initBluetoothClassic();
void updateBluetoothClassic();

#endif // BLE_INTERFACE_H

