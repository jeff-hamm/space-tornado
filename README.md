# Space Tornado - ESP32 Control System

This is the software control system for the Space Tornado rocket vehicle, running on an ESP32 DevKit.

## 🌐 Remote Control

**Control your Space Tornado from your phone:** https://space-tornado.infinitebutts.com

The web app uses Web Bluetooth to connect directly to Space Tornado from Android Chrome or desktop browsers.

## Features

- **Motor Control**: Smooth acceleration/deceleration with logarithmic curve
- **Exhaust System**: Propane thruster control with solenoid and spark igniter
- **Multiple Input Methods**:
  - Physical inputs (potentiometer, buttons, enable switch)
  - Web interface (self-hosted when on same WiFi)
  - Serial/Terminal interface (USB)
  - **BLE** (Bluetooth Low Energy) - works with Web Bluetooth
  - **Bluetooth Classic** (SPP) - works with serial terminal apps
- **Asynchronous Operation**: All systems operate independently

## Hardware Requirements

- ESP32 DevKit V1
- 24V/1000W Battery
- 24V->12V Step Down Converter
- 12V->5V Step Down Converter
- 24V/350W Brushed Motor with Gearbox (10" wheel)
- 24V/350W Motor Speed/Direction Controller
- Propane exhaust system (solenoid, spark igniter, SSR)
- Physical controls (potentiometer, buttons, enable switch)

See [wiring-diagram.md](space-tornado/design/electrical/wiring-diagram.md) for detailed wiring information.

## Pin Assignments

Pin assignments are defined in `include/config.h`. Default assignments:

- **Motor Control**: GPIO 25 (PWM speed), GPIO 26 (direction), GPIO 27 (stop), GPIO 14 (enable)
- **Exhaust**: GPIO 33 (solenoid SSR), GPIO 32 (igniter)
- **Physical Inputs**: GPIO 34 (speed pot), GPIO 35 (direction button), GPIO 39 (fire button), GPIO 36 (enable switch)

**Note**: GPIOs 34, 35, 36, and 39 are input-only on ESP32 and require external 10K pull-up resistors to 3.3V.

## Configuration

Key configuration values in `include/config.h`:

- `MAX_MOTOR_SPEED`: Maximum motor speed percentage (default: 100%)
- `MAX_ACCELERATION`: Maximum acceleration per second (default: 5 percentage points/second)
- `SPEED_INCREMENT`: Speed increment/decrement for terminal commands (default: 10%)
- WiFi settings via build flags in `platformio.ini`

## Building and Uploading

1. Install PlatformIO
2. Open this project in PlatformIO
3. Connect ESP32 via USB
4. Build and upload: `pio run -t upload`
5. Monitor serial output: `pio device monitor`

## Usage

### Physical Controls

- **Speed Potentiometer**: Set target speed (0-100%)
- **Direction Button**: Toggle forward/reverse direction
- **Fire Button**: Hold to fire thrusters (release to stop)
- **Enable Switch**: Must be ON for system to operate

### 📱 Web Bluetooth Control (Recommended for Mobile)

1. Open https://space-tornado.infinitebutts.com on Android Chrome
2. Tap "Connect to Space Tornado"
3. Select "SpaceTornado" from the device list
4. Use the on-screen controls:
   - Speed slider and +/- buttons
   - Forward/Reverse toggle
   - Hold the 🔥 Fire Thrusters button
   - Emergency Stop button

**Note**: iOS Safari does not support Web Bluetooth. Use Bluetooth Classic or WiFi instead.

### Web Interface (WiFi)

1. Connect to WiFi (see WiFi setup below)
2. Access web interface at `http://<esp32-ip-address>/`
3. View status and control speed, direction, and thrusters

### Serial/Terminal Commands

- `+`: Increase speed by 10%
- `-`: Decrease speed by 10%
- `D`: Set direction to FORWARD
- `R`: Set direction to REVERSE
- `F`: Fire thrusters
- `X`: Emergency stop

### Bluetooth Classic (SPP)

For serial Bluetooth terminal apps (works on iOS!):

1. Pair with "SpaceTornado-SPP" Bluetooth device
2. Open your serial terminal app
3. Send same commands as serial interface
4. Send `?` for status query

### BLE Commands

The BLE interface accepts:
- Single character commands: `+`, `-`, `D`, `R`, `F`, `f`, `X`, `C`, `?`
- Speed command: `S##` (e.g., `S50` = set speed to 50%)

## Bluetooth Details

| Interface | Device Name | Use Case |
|-----------|-------------|----------|
| BLE | SpaceTornado | Web Bluetooth, BLE apps |
| Classic SPP | SpaceTornado-SPP | Serial terminal apps |

BLE Service UUID: `4fafc201-1fb5-459e-8fcc-c5c9c331914b`

## WiFi Setup

On first boot, if no WiFi credentials are saved, the ESP32 will create an access point:

- **SSID**: `Space-Tornado-Setup`
- **Password**: `tornado123`

Connect to this network and navigate to `http://192.168.4.1/` to configure WiFi credentials.

## Acceleration System

The motor uses a logarithmic acceleration curve for smooth speed changes:

- Target speed is set immediately when inputs change
- Current speed gradually accelerates/decelerates toward target
- Maximum acceleration is capped by `MAX_ACCELERATION` (percentage points per second)
- Direction changes are applied immediately (no acceleration curve for direction)

## Safety Features

- Enable switch must be ON for system operation
- Emergency stop (`X` command) immediately stops motor and disables system
- Thrusters cannot fire if system is disabled or emergency stop is active
- Motor controller STOP pin is activated during emergency stop or when disabled

## Project Structure

- `src/`: Source files
  - `main.cpp`: Main entry point and loop
  - `rocket_state.cpp`: State management
  - `motor_control.cpp`: Motor acceleration and control
  - `physical_inputs.cpp`: Physical input handling
  - `exhaust_control.cpp`: Exhaust system control
  - `web_interface.cpp`: Web server and API
  - `serial_interface.cpp`: Serial terminal interface
  - `ble_interface.cpp`: Bluetooth interface
  - `wifi_manager.cpp`: WiFi and OTA management
  - `logging.cpp`: Logging system
- `include/`: Header files
- `platformio.ini`: PlatformIO configuration

## License

[Your license here]

