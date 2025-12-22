# Space Tornado - Electrical Wiring Diagram

## System Overview

```
┌─────────────────────────────────────────────────────────────────────────────────────────────┐
│                              SPACE TORNADO ELECTRICAL SYSTEM                                 │
├─────────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                             │
│  ┌──────────────────┐                                                                       │
│  │   24V / 1000W    │───────────────────────────────┬───────────────────────────────────┐   │
│  │     BATTERY      │                               │                                   │   │
│  └────────┬─────────┘                               │                                   │   │
│           │                                         │                                   │   │
│     ┌─────┴─────┐                             ┌─────┴─────┐                             │   │
│     │  24V BUS  │                             │ 24V->12V  │                             │   │
│     │    (+)    │                             │ STEP-DOWN │                             │   │
│     └─────┬─────┘                             └─────┬─────┘                             │   │
│           │                                         │                                   │   │
│           │                               ┌─────────┴─────────┐                         │   │
│           │                               │                   │                         │   │
│     ┌─────┴─────┐                   ┌─────┴─────┐       ┌─────┴─────┐                   │   │
│     │  MOTOR    │                   │  12V BUS  │       │ 12V->5V   │                   │   │
│     │CONTROLLER │                   │    (+)    │       │ STEP-DOWN │                   │   │
│     └─────┬─────┘                   └─────┬─────┘       └─────┬─────┘                   │   │
│           │                               │                   │                         │   │
│     ┌─────┴─────┐                   ┌─────┴─────┐       ┌─────┴─────┐                   │   │
│     │  24V/350W │                   │    SSR    │       │   ESP32   │                   │   │
│     │   MOTOR   │                   │ + SOLENOID│       │  + INPUTS │                   │   │
│     └───────────┘                   └─────┬─────┘       └───────────┘                   │   │
│                                           │                                             │   │
│                                     ┌─────┴─────┐                                       │   │
│                                     │  EXHAUST  │                                       │   │
│                                     │  ASSEMBLY │                                       │   │
│                                     └───────────┘                                       │   │
│                                                                                         │   │
└─────────────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 1. Power Distribution

```
                                    ┌─────────────────────────────────────────────────┐
                                    │              POWER DISTRIBUTION                  │
                                    └─────────────────────────────────────────────────┘

    ╔═══════════════════════╗
    ║   24V / 1000W BATTERY ║
    ║      (Main Power)     ║
    ╚═══════════╦═══════════╝
                ║
      ┌─────────╨─────────┐
      │                   │
  (+) ●═══════════════════●═══════════════════════════════════════════════════════╗
      │                   │                                                       ║
      │    ┌──────────────┴──────────────┐                                        ║
      │    │        24V BUS BAR (+)      │◄────────────────────────────────────┐  ║
      │    │   (Terminal Block/Bus)      │                                     │  ║
      │    └────────────┬─┬──────────────┘                                     │  ║
      │                 │ │                                                    │  ║
      │     ┌───────────┘ └───────────┐                                        │  ║
      │     │                         │                                        │  ║
      │     ▼                         ▼                                        │  ║
      │  ┌──────────────────┐   ┌─────────────────────┐                        │  ║
      │  │ Motor Controller │   │   24V → 12V         │                        │  ║
      │  │   Power (+)      │   │   Step-Down         │                        │  ║
      │  │ (Red/+ terminal) │   │   Converter         │                        │  ║
      │  └────────┬─────────┘   │ (e.g., LM2596 24V)  │                        │  ║
      │           │             └─────────┬───────────┘                        │  ║
      │           │                       │                                    │  ║
      │           ▼                       ▼ 12V Output                         │  ║
      │  ┌──────────────────┐   ┌─────────────────────┐                        │  ║
      │  │   24V/350W       │   │    12V BUS BAR (+)  │                        │  ║
      │  │  Brushed Motor   │   │                     │                        │  ║
      │  │   + Gearbox      │   └────────┬────────────┘                        │  ║
      │  │   (10" Wheel)    │            │                                     │  ║
      │  └──────────────────┘            │                                     │  ║
      │                        ┌─────────┴──────────┐                          │  ║
      │                        │                    │                          │  ║
      │                        ▼                    ▼                          │  ║
      │              ┌──────────────────┐  ┌───────────────────┐               │  ║
      │              │   SSR + 12V      │  │   12V → 5V        │               │  ║
      │              │   Solenoid       │  │   Step-Down       │               │  ║
      │              │   + Igniter      │  │   Converter       │               │  ║
      │              └──────────────────┘  │ (e.g., LM7805)    │               │  ║
      │                                    └─────────┬─────────┘               │  ║
      │                                              │                         │  ║
      │                                              ▼                         │  ║
      │                                    ┌───────────────────┐               │  ║
      │                                    │   5V BUS          │               │  ║
      │                                    │   - +5V Wire      │               │  ║
      │                                    │   - USB-A Port    │               │  ║
      │                                    └─────────┬─────────┘               │  ║
      │                                              │                         │  ║
      │                                              ▼                         │  ║
      │                                    ┌───────────────────┐               │  ║
      │                                    │     ESP32         │               │  ║
      │                                    │   VIN Pin         │               │  ║
      │                                    └───────────────────┘               │  ║
      │                                                                        │  ║
  (-) ●════════════════════════════════════════════════════════════════════════╝  ║
      │                                                                           ║
      │    ┌──────────────────────────────────────────────────────────────────────╝
      │    │
      │    │  ┌──────────────────────────────┐
      │    └──►      24V BUS BAR (-)         │
      │       │  (COMMON GROUND - STAR)      │◄──┬──┬──┬──┬──┬──┐
      │       └──────────────────────────────┘   │  │  │  │  │  │
      │                                          │  │  │  │  │  │
      └──────────────────────────────────────────┘  │  │  │  │  │
                                                    │  │  │  │  │
    ┌───────────────────────────────────────────────┘  │  │  │  │
    │  ┌───────────────────────────────────────────────┘  │  │  │
    │  │  ┌───────────────────────────────────────────────┘  │  │
    │  │  │  ┌───────────────────────────────────────────────┘  │
    │  │  │  │  ┌───────────────────────────────────────────────┘
    │  │  │  │  │
    ▼  ▼  ▼  ▼  ▼
   Motor 12V  12V→5V  SSR   ESP32
   Ctrl  Conv  Conv  GND    GND
```

### Grounding Notes
- **STAR GROUNDING**: All grounds return to a single point (24V negative bus bar)
- This prevents ground loops and ensures reliable operation
- Run separate ground wires from each major component back to the main ground bus

---

## 2. Motor Control System

```
                               ┌─────────────────────────────────────────────────┐
                               │           MOTOR CONTROL WIRING                  │
                               └─────────────────────────────────────────────────┘

    ┌────────────────────────────────────────────────────────────────────────────────────────┐
    │                        24V/350W MOTOR SPEED/DIRECTION CONTROLLER                        │
    │                                                                                        │
    │   POWER SIDE                                           CONTROL SIDE                    │
    │  ┌─────────┐                                          ┌─────────────────────────────┐  │
    │  │ +24V IN │◄────── From 24V Bus (+)                  │ +5V VCC │◄── Motor Ctrl 5V  │  │
    │  │ GND  IN │◄────── From 24V Bus (-)                  │ GND     │◄── Common GND     │  │
    │  │         │                                          │ SPEED   │◄── 0-5V PWM       │  │
    │  │  M+     │────►─┐                                   │ DIR     │◄── Direction      │  │
    │  │  M-     │───►──┼──►  MOTOR                         │ STOP    │◄── Force Stop     │  │
    │  └─────────┘      │                                   │ ENABLE  │◄── Enable Control │  │
    │                   │                                   └─────────────────────────────┘  │
    └───────────────────┼────────────────────────────────────────────────────────────────────┘
                        │
                        ▼
              ┌───────────────────┐
              │   24V/350W MOTOR  │
              │   + GEARBOX       │
              │                   │
              │  ┌─────┐   ┌────┐ │
              │  │  M+ │   │ M- │ │
              │  └──┬──┘   └──┬─┘ │
              │     │         │   │
              └─────┼─────────┼───┘
                    │         │
                    ▼         ▼
              Red Wire    Black Wire
              (from M+)   (from M-)


    ┌─────────────────────────────────────────────────────────────────────────────────────────┐
    │                              ESP32 → MOTOR CONTROLLER WIRING                            │
    │                                                                                         │
    │   ESP32 DevKit                               Motor Controller                           │
    │  ┌──────────────┐                           ┌────────────────┐                          │
    │  │              │                           │                │                          │
    │  │   GPIO 25 ───┼───────[LEVEL SHIFT]──────►│ SPEED (0-5V)   │  ⚠️ SEE NOTE BELOW       │
    │  │   (PWM)      │                           │                │                          │
    │  │              │                           │                │                          │
    │  │   GPIO 26 ───┼──────────────────────────►│ DIRECTION      │  LOW=Reverse, HIGH=Fwd   │
    │  │              │                           │                │                          │
    │  │   GPIO 27 ───┼──────────────────────────►│ STOP           │  HIGH=Force Stop         │
    │  │              │                           │                │                          │
    │  │   GPIO 14 ───┼──────────────────────────►│ ENABLE         │  LOW=Enabled             │
    │  │              │                           │                │                          │
    │  │      GND ────┼──────────────────────────►│ GND            │                          │
    │  │              │                           │                │                          │
    │  └──────────────┘                           └────────────────┘                          │
    │                                                                                         │
    └─────────────────────────────────────────────────────────────────────────────────────────┘


    ⚠️  VOLTAGE LEVEL SHIFTING FOR SPEED CONTROL (GPIO25 → 0-5V)
    ════════════════════════════════════════════════════════════

    Option A: Direct Connection (if motor controller accepts 3.3V as "full speed")
    ─────────────────────────────────────────────────────────────────────────────

         ESP32 GPIO 25 ────────────────────────────► Motor Controller SPEED Input
            (PWM)                                        (High Impedance)

         Note: Many motor controllers accept 3.3V as max. Test first!


    Option B: Non-Inverting Op-Amp Level Shifter (Recommended)
    ───────────────────────────────────────────────────────────

                            ┌────────────────┐
                            │   +5V Supply   │
                            └───────┬────────┘
                                    │
                               ┌────┴────┐
                               │    8    │ V+
                               │         │
        ESP32 GPIO25 ──[10K]───┤2-   LM358├───►  Motor Controller
           (PWM)               │    1    │       SPEED Input
                        ┌──────┤3+       │
                        │      │         │
                       [R1]    │    4    │ V-
                       10K     └────┬────┘
                        │           │
                       [R2]         │
                       15K      ┌───┴───┐
                        │       │  GND  │
                        └───────┴───────┘

        Gain = 1 + (R2/R1) = 1 + (15K/10K) = 2.5x
        3.3V × 2.5 = 8.25V (clamped to 5V by supply)


    Option C: Dedicated Level Shifter Module (Easiest)
    ────────────────────────────────────────────────────

        ESP32                  Level Shifter             Motor Controller
      ┌──────────┐            ┌────────────┐            ┌────────────────┐
      │ GPIO 25 ─┼───────────►│ LV1   HV1 ─┼───────────►│ SPEED          │
      │ 3.3V ────┼───────────►│ LV    HV ◄─┼────────────│ +5V VCC        │
      │ GND ─────┼───────────►│ GND   GND ─┼───────────►│ GND            │
      └──────────┘            └────────────┘            └────────────────┘

                              e.g., TXS0108E or BSS138-based module
```

---

## 3. Exhaust / Propane System

```
                               ┌─────────────────────────────────────────────────┐
                               │            EXHAUST SYSTEM WIRING                 │
                               └─────────────────────────────────────────────────┘


    FUEL FLOW PATH
    ══════════════

    ┌─────────────┐      ┌─────────────┐      ┌─────────────┐      ┌─────────────┐
    │   PROPANE   │      │   NEEDLE    │      │    BALL     │      │    12V      │
    │   TANK      │─────►│   VALVE     │─────►│   SWITCH    │─────►│  SOLENOID   │
    │   (1 lb)    │      │  (Pressure) │      │  (Manual    │      │  (Electric  │
    │             │      │             │      │   Safety)   │      │   Shutoff)  │
    └─────────────┘      └─────────────┘      └─────────────┘      └──────┬──────┘
                                                                          │
                                                                          ▼
                                                              ┌───────────────────────┐
                                                              │  90° 1/4" NPT Elbow   │
                                                              └───────────┬───────────┘
                                                                          │
                                                                          ▼
                                                              ┌───────────────────────┐
                                                              │  4" 1/4" NPT Nipple   │
                                                              │     (Outlet Tube)     │
                                                              └───────────┬───────────┘
                                                                          │
                                                                          ▼
                                                              ┌───────────────────────┐
                                                              │    SPARK IGNITER      │
                                                              │   (12V → 10kV Arc)    │
                                                              └───────────────────────┘


    ELECTRICAL CONTROL - SOLENOID CIRCUIT
    ══════════════════════════════════════

    12V Bus (+) ──────────────────────────────────────────┐
                                                          │
                                                          ▼
                                               ┌──────────────────┐
                                               │   12V SOLENOID   │
                                               │   (Normally      │
                                               │    Closed)       │
                                               │   ●──── IN (+)   │
                                               │   ●──── OUT (-)  │
                                               └────────┬─────────┘
                                                        │
       ┌────────────────────────────────────────────────┘
       │
       ▼
    ┌──────────────────────────────────────────────────┐
    │             SOLID STATE RELAY (SSR)              │
    │           (e.g., Fotek SSR-40DA)                 │
    │                                                  │
    │   LOAD SIDE              CONTROL SIDE            │
    │  ┌────────┐              ┌────────┐              │
    │  │ DC+    │◄─────────────┤ (+)    │◄── ESP32 GPIO 33
    │  │ DC-    │──────────────►(-)     │◄── ESP32 GND │
    │  └────────┘              └────────┘              │
    │      │                                           │
    └──────┼───────────────────────────────────────────┘
           │
           ▼
        12V Bus (-)


    ESP32 TO SSR + IGNITER WIRING
    ══════════════════════════════

    ESP32 DevKit
   ┌────────────────┐
   │   GPIO 33 ─────┼──────────►  SSR Control (+)
   │  (Solenoid)    │
   │      GND ──────┼──────────►  SSR Control (-)
   │                │
   │   GPIO 32 ─────┼──────────►  Igniter Module Trigger (+)
   │   (Igniter)    │
   │      GND ──────┼──────────►  Igniter Module GND
   └────────────────┘

   ⚠️  IGNITER NOTE: If igniter is just 12V-powered, use a second
       SSR or MOSFET to switch its 12V power via GPIO 32.


    ALTERNATIVE: MOSFET-BASED SWITCHING
    ════════════════════════════════════

                          12V Bus (+)
                              │
                    ┌─────────┴─────────┐
                    │                   │
                    ▼                   │
              ┌───────────┐             │
              │ SOLENOID  │             │
              │   12V     │             │
              └─────┬─────┘             │
                    │  ┌───────────┐    │
                    │  │  Flyback  │    │
                    │  │   Diode   │    │
                    │  │  1N4007   │    │
                    │  └─────┬─────┘    │
                    └────────┼──────────┘
                             │ Drain
                         ┌───┴───┐
    ESP32 GPIO 33 ──[1K]─┤ GATE  │  IRLZ44N (Logic-Level MOSFET)
                         └───┬───┘
                             │ Source
                             ▼
                         GND (Common)
```

---

## 4. Physical Input Controls

```
                               ┌─────────────────────────────────────────────────┐
                               │          PHYSICAL INPUT WIRING                   │
                               └─────────────────────────────────────────────────┘


    ⚠️  IMPORTANT: GPIO 34, 35, 36, 39 are INPUT-ONLY with NO internal pull-up!
        You MUST use external 10K pull-up resistors to 3.3V for buttons/switches.


    SPEED POTENTIOMETER (Analog Input)
    ═══════════════════════════════════

                     3.3V
                      │
              ┌───────┴───────┐
              │    10K POT    │
              │   ┌───────┐   │
              └───┤ WIPER ├───┴───────────► GPIO 34 (ADC)
                  └───────┘
                      │
                      ▼
                     GND


    DIRECTION BUTTON (Momentary, Normally Open)
    ════════════════════════════════════════════

                     3.3V
                      │
                     [10K]  ◄── External Pull-up (REQUIRED!)
                      │
                      ├─────────────────────► GPIO 35
                      │
                   ┌──┴──┐
                   │ DIR │   Momentary Push Button
                   └──┬──┘
                      ▼
                     GND


    FIRE THRUSTERS BUTTON (Momentary, Normally Open)
    ═════════════════════════════════════════════════

                     3.3V
                      │
                     [10K]  ◄── External Pull-up (REQUIRED!)
                      │
                      ├─────────────────────► GPIO 39 (VN)
                      │
                   ┌──┴──┐
                   │FIRE │   BIG RED BUTTON 🔴
                   └──┬──┘
                      ▼
                     GND


    ENABLE SWITCH (Toggle, SPST)
    ═════════════════════════════

                     3.3V
                      │
                     [10K]  ◄── External Pull-up (REQUIRED!)
                      │
                      ├─────────────────────► GPIO 36 (VP)
                      │
                   ┌──┴──┐
                   │ EN  │   Toggle Switch (ON = LOW)
                   └──┬──┘
                      ▼
                     GND


    COMBINED SCHEMATIC
    ═══════════════════

                                          3.3V Rail
                                             │
           ┌─────────────────────────────────┼────────────────────────────────────┐
           │            ┌────────────────────┼────────────────────┐               │
          [10K]        [10K]                [10K]             ┌───┴───┐           │
           │            │                    │                │ 10K   │           │
           ├───►GPIO35  ├───►GPIO39          ├───►GPIO36      │  POT  │           │
           │            │                    │                │ WIPER ├──►GPIO34  │
        ┌──┴──┐      ┌──┴──┐              ┌──┴──┐             └───┬───┘           │
        │ DIR │      │FIRE │              │ EN  │                 │               │
        └──┬──┘      └──┬──┘              └──┬──┘                 │               │
           └────────────┴────────────────────┴────────────────────┴───────────────┘
                                             │
                                            GND
```

---

## 5. ESP32 Pin Assignment Summary

```
                               ┌─────────────────────────────────────────────────┐
                               │              ESP32 DEVKIT PINOUT                 │
                               └─────────────────────────────────────────────────┘

                                    ┌─────────────────────┐
                                    │     ESP32 DEVKIT    │
                          EN  ──────┤ EN            D23   ├──────
                         VP  ───────┤ VP/36    ▲    D22   ├────── (I2C SCL)
                         VN  ───────┤ VN/39    │    TX0   ├────── Serial TX
                         D34 ───────┤ D34      │    RX0   ├────── Serial RX
                         D35 ───────┤ D35      │    D21   ├────── (I2C SDA)
                         D32 ───────┤ D32      │    D19   ├──────
                         D33 ───────┤ D33      │    D18   ├──────
                         D25 ───────┤ D25      │    D5    ├──────
                         D26 ───────┤ D26      │    D17   ├──────
                         D27 ───────┤ D27      │    D16   ├──────
                         D14 ───────┤ D14      │    D4    ├──────
                         D12 ───────┤ D12      │    D2    ├────── (LED)
                         D13 ───────┤ D13      │    D15   ├──────
                         GND ───────┤ GND      │    GND   ├────── GND
                         VIN ───────┤ VIN      │    3V3   ├────── 3.3V
                                    │     [USB PORT]      │
                                    └─────────────────────┘


    ╔═══════════════════════════════════════════════════════════════════════════════════════╗
    ║                               PIN ASSIGNMENT TABLE                                     ║
    ╠════════════╦═══════════════════════════════╦══════════════════════════════════════════╣
    ║   GPIO     ║   FUNCTION                    ║   NOTES                                  ║
    ╠════════════╬═══════════════════════════════╬══════════════════════════════════════════╣
    ║   POWER    ║                               ║                                          ║
    ║   VIN      ║   5V Power Input              ║   From 12V→5V step-down                  ║
    ║   GND      ║   Ground                      ║   Common ground (star topology)          ║
    ╠════════════╬═══════════════════════════════╬══════════════════════════════════════════╣
    ║   MOTOR    ║                               ║                                          ║
    ║   GPIO 25  ║   Motor Speed (PWM)           ║   Needs level shift to 5V                ║
    ║   GPIO 26  ║   Motor Direction             ║   HIGH=Forward, LOW=Reverse              ║
    ║   GPIO 27  ║   Motor STOP                  ║   HIGH=Force Stop                        ║
    ║   GPIO 14  ║   Motor ENABLE                ║   LOW=Enabled                            ║
    ╠════════════╬═══════════════════════════════╬══════════════════════════════════════════╣
    ║   EXHAUST  ║                               ║                                          ║
    ║   GPIO 33  ║   SSR Trigger (Solenoid)      ║   HIGH=Open solenoid                     ║
    ║   GPIO 32  ║   Igniter Trigger             ║   HIGH=Spark on                          ║
    ╠════════════╬═══════════════════════════════╬══════════════════════════════════════════╣
    ║   INPUTS   ║   (ALL NEED EXTERNAL PULLUP)  ║                                          ║
    ║   GPIO 34  ║   Speed Potentiometer (ADC)   ║   Analog input 0-3.3V                    ║
    ║   GPIO 35  ║   Direction Button            ║   10K pull-up to 3.3V REQUIRED           ║
    ║   GPIO 39  ║   Fire Thrusters Button       ║   10K pull-up to 3.3V REQUIRED           ║
    ║   GPIO 36  ║   Enable Switch               ║   10K pull-up to 3.3V REQUIRED           ║
    ╠════════════╬═══════════════════════════════╬══════════════════════════════════════════╣
    ║   COMMS    ║                               ║                                          ║
    ║   TX0/RX0  ║   Serial (via USB)            ║   Built-in USB-to-Serial                 ║
    ║   (int)    ║   WiFi                        ║   Built-in WiFi antenna                  ║
    ║   (int)    ║   Bluetooth Classic + BLE     ║   Built-in dual-mode Bluetooth           ║
    ╚════════════╩═══════════════════════════════╩══════════════════════════════════════════╝
```

---

## 6. Complete System Wiring

```
═══════════════════════════════════════════════════════════════════════════════════════════════════
                                    COMPLETE SYSTEM WIRING
═══════════════════════════════════════════════════════════════════════════════════════════════════

        ┌─────────────────────────────────────────────────────────────────────────────────────────┐
        │                                    24V SECTION                                          │
        │    ╔═══════════════════╗                                                                │
        │    ║   24V / 1000W     ║                                                                │
        │    ║     BATTERY       ║                                                                │
        │    ╚═════════╦═════════╝                                                                │
        │              ║                                                                          │
        │      (+)═════╬══════════════════════════════════════════════════════════════(+)         │
        │              ║          ┌───────────────────────────────────────────────┐    │          │
        │              ╠══════════►              24V BUS BAR (+)                  │◄═══╝          │
        │              ║          └───────────────────┬───────────────────────────┘               │
        │              ║                ┌─────────────┴─────────────┐                             │
        │              ║                ▼                           ▼                             │
        │              ║    ┌───────────────────────┐   ┌───────────────────────┐                 │
        │              ║    │   MOTOR CONTROLLER    │   │     24V → 12V         │                 │
        │              ║    │       (+24V IN)       │   │     STEP-DOWN         │                 │
        │              ║    └───────────┬───────────┘   └───────────┬───────────┘                 │
        │              ║                ▼                           │                             │
        │              ║    ┌───────────────────────┐               │                             │
        │              ║    │   24V/350W MOTOR      │               │                             │
        │              ║    └───────────────────────┘               │                             │
        │      (-)═════╬══════════════════════════════════════════════════════════════(-)         │
        │              ║          ┌───────────────────────────────────────────────┐    ▲          │
        │              ╠══════════►      24V BUS BAR (-)  [STAR GND]              │◄═══╝          │
        │              ║          └───────────────────────────────────────────────┘               │
        └─────────────────────────────────────────────────────────────────────────────────────────┘

        ┌─────────────────────────────────────────────────────────────────────────────────────────┐
        │                                    12V SECTION                                          │
        │              From 24V→12V Step-Down                                                     │
        │          ┌───────────────────────────────────────────────────┐                          │
        │          │                  12V BUS BAR (+)                  │                          │
        │          └──────────────────────┬────────────────────────────┘                          │
        │                   ┌─────────────┼─────────────┐                                         │
        │                   ▼             ▼             ▼                                         │
        │        ┌───────────────┐ ┌────────────┐ ┌────────────────┐                              │
        │        │  SSR + 12V    │ │  IGNITER   │ │  12V → 5V      │                              │
        │        │  SOLENOID     │ │   +12V     │ │  STEP-DOWN     │                              │
        │        └───────────────┘ └────────────┘ └───────┬────────┘                              │
        │                                                 ▼                                       │
        │                                         ┌───────────────┐                               │
        │                                         │   5V OUTPUT   │───► ESP32 VIN                 │
        │                                         └───────────────┘                               │
        └─────────────────────────────────────────────────────────────────────────────────────────┘

        ┌─────────────────────────────────────────────────────────────────────────────────────────┐
        │                                   ESP32 CONNECTIONS                                     │
        │                           ┌─────────────────────────────┐                               │
        │                           │         ESP32 DEVKIT        │                               │
        │   From 5V Step-Down ─────►│ VIN                    GND  │◄───── To 24V GND Bus          │
        │                           │                             │                               │
        │   ┌───────────────────────┤ GPIO 25 (PWM) ══[LEVEL]══►──┼───── Motor SPEED              │
        │   │  MOTOR            ┌───┤ GPIO 26 ────────────────────┼───── Motor DIRECTION          │
        │   │  CONTROLLER       │   │ GPIO 27 ────────────────────┼───── Motor STOP               │
        │   │  CONTROL ─────────┴───┤ GPIO 14 ────────────────────┼───── Motor ENABLE             │
        │   │                   ┌───┤ GPIO 33 ────────────────────┼───── SSR Control (+)          │
        │   │  EXHAUST ─────────┴───┤ GPIO 32 ────────────────────┼───── Igniter Control          │
        │   │                   ┌───┤ GPIO 34 (ADC) ◄─────────────┼───── Speed POT Wiper          │
        │   │  PHYSICAL         │   │ GPIO 35 ◄───[10K]───3.3V ───┼───── Direction Button         │
        │   │  INPUTS ──────────┤   │ GPIO 39 ◄───[10K]───3.3V ───┼───── Fire Button              │
        │   │                   └───┤ GPIO 36 ◄───[10K]───3.3V ───┼───── Enable Switch            │
        │   │                       │         [USB PORT]          │                               │
        │   │                       └─────────────────────────────┘                               │
        │   └─────────────────────────────► Motor Controller GND ─────► 24V GND Bus               │
        └─────────────────────────────────────────────────────────────────────────────────────────┘
═══════════════════════════════════════════════════════════════════════════════════════════════════
```

---

## 7. Safety Considerations

### Power System
- ⚡ **Main Fuse**: Install 50A fuse on 24V battery positive line
- ⚡ **Branch Fuses**: 20A motor, 10A 12V bus, 5A 5V bus

### Motor Control
- 🔄 **Flyback Protection**: Motor controller should have built-in protection
- 🛑 **Emergency Stop**: Physical E-STOP button recommended

### Exhaust System
- 🔥 **Propane Safety**: Close ball valve when not in use
- 💥 **Ignition**: Igniter should pulse, not run continuously; keep wires away from propane

### ESP32
- 📍 **Input Protection**: GPIO 34-39 are input-only; 3.3V max
- 📍 **Output Protection**: Never connect 5V directly to any GPIO

---

## 8. Bill of Materials (BOM)

| Qty | Component | Specifications | Notes |
|-----|-----------|----------------|-------|
| 1 | Battery | 24V, 1000W (40+ Ah) | LiFePO4 recommended |
| 1 | DC-DC Converter | 24V→12V, 10A+ | Buck converter |
| 1 | DC-DC Converter | 12V→5V, 3A+ | For ESP32 + USB |
| 1 | Motor | 24V/350W Brushed | With gearbox, 10" wheel |
| 1 | Motor Controller | 24V/350W | With 0-5V speed input |
| 1 | ESP32 DevKit | Any variant | DOIT recommended |
| 1 | SSR | 3-32VDC control | Fotek SSR-40DA or similar |
| 1 | 12V Solenoid | NC, 1/4" NPT | Brass body preferred |
| 1 | Spark Igniter | 12V→10kV | Continuous duty |
| 1 | Propane Tank | 1lb | Standard camping size |
| 1 | Needle Valve | 1/4" NPT | Brass |
| 1 | Ball Valve | 1/4" NPT | Quarter-turn |
| 1 | NPT Elbow | 90°, 1/4" | Brass |
| 1 | NPT Nipple | 4", 1/4" | Brass |
| 1 | Potentiometer | 10K linear | Panel mount |
| 2 | Push Buttons | Momentary NO | Panel mount |
| 1 | Toggle Switch | SPST | Panel mount |
| 4 | Resistors | 10K, 1/4W | External pull-ups |
| 1 | Level Shifter | 3.3V↔5V | TXS0108E or BSS138 |
| 2 | Bus Bars | 4+ terminals | Power distribution |

### Wire Gauges
| Circuit | Current | AWG |
|---------|---------|-----|
| 24V Battery | 40A+ | 8 |
| Motor Controller | 15A | 14 |
| 12V Distribution | 10A | 16 |
| 5V/Signal | <1A | 22-24 |

