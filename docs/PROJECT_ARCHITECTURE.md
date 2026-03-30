# MiniSumo2026 Project Architecture Documentation

## 📋 Table of Contents

1. [System Overview](#system-overview)
2. [Class Architecture](#class-architecture)
3. [System Flow](#system-flow)
4. [Component Details](#component-details)
5. [Hardware Pin Configuration](#hardware-pin-configuration)
6. [State Management](#state-management)

---

## System Overview

The MiniSumo2026 is a small sumo robot built on a **Raspberry Pi Pico** microcontroller. The system uses infrared sensors to detect opponents, line sensors to prevent falling off the ring, and a dual-motor drive system for movement. A menu system allows configuration via a single button, and an OLED display provides visual feedback.

**Key Specifications:**

- **Microcontroller:** Raspberry Pi Pico (RP2040)
- **Motor Driver:** DRV8243 (dual motor control)
- **Display:** Adafruit SSD1306 128x64 OLED over I2C
- **Main Loop Frequency:** ~200 Hz (5ms delay per cycle)
- **Display Update Rate:** ~33 FPS (30ms throttle)

---

## Class Architecture

### System Class Diagram

```mermaid
graph TB
    subgraph Core["Core System"]
        Robot["🤖 Robot"]
        Main["main.cpp<br/>(Arduino Entry)"]
    end

    subgraph Motors["Motor Control"]
        Motor["⚙️ Motor<br/>DRV8243 Driver<br/>PWM Control"]
    end

    subgraph Sensors["Sensor Systems"]
        IR["🔍 IRSensors<br/>3 Proximity Sensors<br/>Digital Reads"]
        QTR["📍 QTRSensors<br/>2 Line Sensors<br/>Digital Reads"]
    end

    subgraph Input["Input System"]
        Button["🔘 ButtonManager<br/>Debouncing<br/>Gesture Detection"]
    end

    subgraph Display["Display & UI"]
        Disp["🖥️ Display<br/>SSD1306 OLED<br/>Menu System"]
        Menu["📑 Menu Definitions<br/>RobotMode<br/>SpeedLevel"]
    end

    subgraph Config["Configuration"]
        Defines["⚙️ Defines<br/>SpeedConfig<br/>Pin Definitions"]
    end

    subgraph Audio["Audio Output"]
        Melody["🎵 Melody<br/>Buzzer Control<br/>Tone Generation"]
    end

    Main -->|instantiates| Robot

    Robot -->|contains| Motor
    Robot -->|contains| Disp
    Robot -->|contains| IR
    Robot -->|reads| Button

    Motor -->|uses| Defines
    Robot -->|uses| Menu
    Robot -->|uses| Melody
    Robot -->|uses| QTR
    Disp -->|uses| Menu
    Disp -->|uses| Defines

    classDef core fill:#FF6B6B,stroke:#333,stroke-width:2px,color:#fff
    classDef sensor fill:#4ECDC4,stroke:#333,stroke-width:2px,color:#fff
    classDef motor fill:#95E1D3,stroke:#333,stroke-width:2px,color:#333
    classDef ui fill:#FFE66D,stroke:#333,stroke-width:2px,color:#333
    classDef config fill:#A8E6CF,stroke:#333,stroke-width:2px,color:#333

    class Robot,Main core
    class IR,QTR,Button sensor
    class Motor motor
    class Disp,Menu ui
    class Defines,Melody config
```

### Detailed Class Relationships

```mermaid
classDiagram
    class Robot {
        - motor: Motor
        - display: Display
        - irSensors: IRSensors
        - currentMode: RobotMode
        - currentMenuScreen: int
        - paused: bool
        - currentSpeedLevel: int
        - speedConfig: SpeedConfig
        +setup()
        +update()
        +handleButtonGesture(gesture)
        +getMode()
        +setMode(mode)
        +getIRValues()
        +getDisplay()
        +togglePause()
        -updateBehavior()
        -applySpeedPreset(level)
    }

    class Motor {
        - ALPHA_FILTER: float
        - filteredCurrent_A: float
        - filteredCurrent_B: float
        - peakCurrent_A: float
        - peakCurrent_B: float
        +setup()
        +forward(pwm)
        +backward(pwm)
        +left(pwm)
        +right(pwm)
        +stop()
        +readMotorCurrent()
        +readMotorBCurrent()
        +getFilteredMotorCurrent()
        +getFilteredMotorBCurrent()
        +updatePeaks()
        -initDRV8243()
    }

    class Display {
        - display: Adafruit_SSD1306
        - lastUpdateTime: unsigned long
        +setup(): bool
        +print(text, value)
        +clear()
        +drawLoadingScreen(status)
        +drawMainScreen()
        +drawCurentReading(motorA, motorB)
        +displayIR(irValues, count)
        +shouldUpdate(): bool
        +drawSpeedSelectorScreen(level)
        +drawSettingsScreen(status)
        +drawSensorReadingsScreen(values)
        -drawBarGraph(values, count)
    }

    class IRSensors {
        - sensorValues[3]: int
        +setup()
        +read()
        +getValue(index): int
        +getAllValues(): int*
        +printAllValues()
    }

    class QTRSensors {
        - sensorValues[2]: int
        +setup()
        +read()
        +getValue(index): int
        +getAllValues(): int*
        +printAllValues()
    }

    class ButtonManager {
        - currentState: ButtonState
        - previousState: ButtonState
        - detectedGesture: ButtonGesture
        - lastDebounceTime: unsigned long
        - pressCount: int
        +setup()
        +update()
        +getGesture(): ButtonGesture
        +getState(): ButtonState
        +isPressed(): bool
        -readRawButton(): bool
        -updateStateMachine()
        -detectGesture()
    }

    class Melody {
        +playTone(frequency, duration)
        +playMelody(melody)
    }

    Robot "1" --> "1" Motor
    Robot "1" --> "1" Display
    Robot "1" --> "1" IRSensors
    Robot "1" --> "1" QTRSensors
    Robot "0..*" --> "1" ButtonManager
    Robot "1" --> "1" Melody
```

---

## System Flow

### Main Program Execution Flow

```mermaid
sequenceDiagram
    participant Arduino
    participant Robot as robot.update()
    participant IR as IRSensors
    participant Motor
    participant Display

    Arduino->>Robot: setup() [once]
    activate Robot
    Robot->>Motor: setup()
    Robot->>Display: setup()
    Robot->>IR: setup()
    deactivate Robot

    loop Every 5ms (200Hz)
        Arduino->>Robot: update()
        activate Robot
        Robot->>IR: read()
        activate IR
        IR->>IR: digitalRead(pins)
        deactivate IR
        Robot->>Robot: updateBehavior()
        activate Robot
        Robot->>Motor: forward/backward/left/right()
        Robot->>Motor: stop()
        deactivate Robot
        alt Every 30ms
            Robot->>Display: displayIR()
            activate Display
            Display->>Display: shouldUpdate()
            Display->>Display: render bars
            deactivate Display
        end
        deactivate Robot
        Arduino->>Arduino: delay(5)
    end
```

### Button Gesture Detection & Menu Navigation

```mermaid
stateDiagram-v2
    [*] --> IDLE

    IDLE --> PRESSED: Button Down
    PRESSED --> HELD: Hold > 20ms
    HELD --> RELEASED: Button Up
    RELEASED --> IDLE: Process Gesture

    note right of IDLE
        No button activity
        Display running mode
    end note

    note right of HELD
        Track timing:
        - Single press: 0-400ms
        - Double press: <400ms window
        - Long press: >1000ms
    end note
```

### Robot Operating Modes

```mermaid
stateDiagram-v2
    MODE_RUNNING --> MODE_MENU: Long Press Button
    MODE_RUNNING --> MODE_PAUSED: Single Press Button

    MODE_PAUSED --> MODE_RUNNING: Single Press Button
    MODE_PAUSED --> MODE_MENU: Long Press Button

    MODE_MENU --> MODE_RUNNING: Long Press Button

    note right of MODE_RUNNING
        - IR sensors active
        - Motors responding to sensors
        - Behavior: Attack/Search
    end note

    note right of MODE_PAUSED
        - Sensors still reading
        - Motors disabled (stop)
        - Can view sensor data
    end note

    note right of MODE_MENU
        - Display menus
        - Adjust speed levels
        - View sensor readings
        - View current draw
    end note
```

### Menu Screen Navigation

```mermaid
graph TB
    subgraph Screens["Menu Screens (5 Total)"]
        S0["Screen 0: STATUS<br/>Mode + Pause + Speed"]
        S1["Screen 1: SPEED<br/>Current Speed Level"]
        S2["Screen 2: CURRENT<br/>Motor A/B Current"]
        S3["Screen 3: PEAK CURRENT<br/>Peak A/B + Total"]
        S4["Screen 4: IR SENSORS<br/>Sensor Readings"]
    end

    S0 -->|Double Press| S1
    S1 -->|Double Press| S2
    S2 -->|Double Press| S3
    S3 -->|Double Press| S4
    S4 -->|Double Press| S0

    style S0 fill:#FFE66D
    style S1 fill:#FFE66D
    style S2 fill:#FFE66D
    style S3 fill:#FFE66D
    style S4 fill:#FFE66D
```

### IR Sensor-Based Behavior

```mermaid
graph TB
    START["Robot.updateBehavior()"] --> READ["Read IR Sensors<br/>LEFT, CENTER, RIGHT"]
    READ --> LOGIC["Check 8 Possible Patterns"]

    LOGIC --> P1["All Zero<br/>Search Mode"]
    LOGIC --> P2["Center Only<br/>— ATTACK!"]
    LOGIC --> P3["Side + Center<br/>Sharp Turn"]
    LOGIC --> P4["Side Only<br/>Search Turn"]

    P1 --> APPLY["Apply Speed Config<br/>search_speed"]
    P2 --> APPLY["Apply Speed Config<br/>attack_speed"]
    P3 --> APPLY["Apply Speed Config<br/>turn_aggressive"]
    P4 --> APPLY["Apply Speed Config<br/>turn_moderate"]

    APPLY --> CMD["Command Motor<br/>forward/left/right"]
    CMD --> END["Next 5ms Cycle"]

    style START fill:#FF6B6B,color:#fff
    style END fill:#FF6B6B,color:#fff
    style APPLY fill:#4ECDC4,color:#fff
```

---

## Component Details

### 🔍 Infrared Sensors (IRSensors)

**Purpose:** Detect proximity of opponent within ~10cm

| Property     | Value                                    |
| ------------ | ---------------------------------------- |
| Count        | 3 sensors                                |
| Pin Layout   | LEFT=GPIO12, CENTER=GPIO13, RIGHT=GPIO14 |
| Read Type    | Digital (HIGH/LOW)                       |
| Update Rate  | Every 5ms with robot.update()            |
| Beam Pattern | Forward-facing, ~10cm detection range    |

**Usage in Behavior:**

- **All three detect:** Opponent directly ahead → **Attack forward**
- **Center only:** Opponent straight ahead → **Attack forward**
- **Left + Center:** Opponent on left side → **Sharp left turn**
- **Right + Center:** Opponent on right side → **Sharp right turn**
- **Left or Right only:** Opponent on far side → **Gentle turn + advance**
- **None detect:** Search mode → **Rotate in place**

---

### 📍 Line Sensors (QTRSensors)

**Purpose:** Detect ring boundaries to prevent falling off

| Property   | Value                                   |
| ---------- | --------------------------------------- |
| Count      | 2 sensors                               |
| Pin Layout | LEFT=GPIO2, RIGHT=GPIO3                 |
| Read Type  | Digital (reflectance based)             |
| Mounting   | Front-facing, detecting white ring edge |

**Current Status:** Configured but behavior not yet integrated into updateBehavior()

---

### ⚙️ Motor Control System

**Motor Driver:** DRV8243 Dual H-Bridge

- **Max Current:** 2A per channel
- **Voltage:** 3.3V logic, powered by battery

| Motor     | Control Pins                       | Enable Pin        |
| --------- | ---------------------------------- | ----------------- |
| A (Left)  | GPIO 8 (PWM_A1), GPIO 9 (PWM_A2)   | GPIO 22 (N_SLEEP) |
| B (Right) | GPIO 20 (PWM_B1), GPIO 21 (PWM_B2) | GPIO 22 (N_SLEEP) |

**PWM Values (0-255):**

- 0 = No power
- 128 = 50% power
- 255 = Full power

**Current Sensing:**

- Motor A: GPIO 26 (ADC2) → reads in Amps with alpha filtering
- Motor B: GPIO 27 (ADC3) → reads in Amps with alpha filtering
- Peak current tracking automatically records maximum current draw

**Movement Commands:**

```
forward(pwm)   - Both motors forward
backward(pwm)  - Both motors backward
left(pwm)      - Left motor back, Right motor forward (spin left)
right(pwm)     - Right motor back, Left motor forward (spin right)
stop()         - Both motors off
```

---

### 🖥️ Display System (SSD1306 OLED)

**Hardware:**

- Resolution: 128x64 pixels
- Communication: I2C (SDA=GPIO4, SCL=GPIO5)
- Refresh Throttle: 30ms (~33 FPS)

**Menu Screens:**

| Screen # | Name         | Purpose                                       |
| -------- | ------------ | --------------------------------------------- |
| 0        | STATUS       | Shows current mode, pause status, speed level |
| 1        | SPEED        | Current speed level (LOW/MEDIUM/HIGH)         |
| 2        | CURRENT      | Real-time motor A & B current draw            |
| 3        | PEAK CURRENT | Peak values and total peak current            |
| 4        | IR SENSORS   | IR sensor status as bar graphs                |

**Display Refresh Logic:**

- `shouldUpdate()` checks if ≥30ms since last refresh
- Prevents excessive I2C writes that would slow down main loop
- Actual display updates happen at ~33 FPS max

---

### 🔘 Button Manager System

**Button Hardware:**

- Pin: GPIO 15
- Type: Active-low (INPUT_PULLUP)
- Debounce Time: 20ms
- Uses Timer-based gesture detection

**Gesture Detection:**

| Gesture      | Timing                  | Use                              |
| ------------ | ----------------------- | -------------------------------- |
| Single Press | Press + Release < 400ms | Toggle Pause / Cycle Menu Screen |
| Double Press | 2 presses < 400ms apart | Cycle Speed Level / Next Menu    |
| Long Press   | Press > 1000ms          | Enter/Exit Menu Mode             |

**Button State Machine:**

```
IDLE → PRESSED → HELD → RELEASED → [Gesture Detection] → IDLE
       ↑_________________________________↑
       (debounce 20ms prevents flicker)
```

---

### 🎵 Audio System (Melody)

**Buzzer Hardware:**

- Pin: GPIO 3 (PWM tone output)
- Frequency Range: 20Hz - 20kHz (any audible tone)
- Implementation: Tone generation via PWM

**Usage in Robot:**

- Startup confirmation
- Attack sounds
- Error alerts
- Movement feedback

---

## Hardware Pin Configuration

### Complete Pin Map

```mermaid
graph LR
    subgraph GPIO["GPIO Pins (RP2040)"]
        G0["GPIO 0"]
        G1["GPIO 1"]
        G2["GPIO 2: QTR Left"]
        G3["GPIO 3: Buzzer/QTR Right"]
        G4["GPIO 4: I2C SDA"]
        G5["GPIO 5: I2C SCL"]
        G8["GPIO 8: PWM_A1"]
        G9["GPIO 9: PWM_A2"]
        G12["GPIO 12: IR Left"]
        G13["GPIO 13: IR Center"]
        G14["GPIO 14: IR Right"]
        G15["GPIO 15: Button"]
        G20["GPIO 20: PWM_B1"]
        G21["GPIO 21: PWM_B2"]
        G22["GPIO 22: N_SLEEP"]
        G26["GPIO 26: ADC Current_A"]
        G27["GPIO 27: ADC Current_B"]
    end

    subgraph Used["✓ USED"]
        Used1["Motor PWM Control (8,9,20,21)"]
        Used2["Motor Enable (22)"]
        Used3["IR Proximity (12,13,14)"]
        Used4["Line Sensors (2,3)"]
        Used5["Current Sensing (26,27)"]
        Used6["I2C Display (4,5)"]
        Used7["Buzzer (3)"]
        Used8["Button Input (15)"]
    end

    subgraph Available["⊘ Available"]
        Avail["GPIO 0, 1, 6, 7, 10, 11, 16, 17, 18, 19, 23, 24, 25, 28, 29"]
    end

    style Used1 fill:#4ECDC4
    style Used2 fill:#4ECDC4
    style Used3 fill:#4ECDC4
    style Used4 fill:#4ECDC4
    style Used5 fill:#4ECDC4
    style Used6 fill:#4ECDC4
    style Used7 fill:#4ECDC4
    style Used8 fill:#4ECDC4
```

### Conflict Note ⚠️

**GPIO 3 Conflict:** The QTR Right sensor and Buzzer share GPIO 3. Currently buzzer has priority. To use QTR Right sensor effectively, buzzer would need to be moved to an available GPIO.

---

## State Management

### Speed Configuration System

**Global `SpeedConfig` object** allows menu to adjust behavior in real-time:

```cpp
struct SpeedConfig {
    int attack_speed;          // Speed when attacking (detect center)
    int search_speed;          // Speed when searching
    int turn_speed_aggressive; // Sharp turns (180° detections)
    int turn_speed_moderate;   // Standard turns (side detections)
    int turn_speed_gentle;     // Subtle corrections
};
```

**Speed Presets (3 Levels):**

| Aspect        | LOW | MEDIUM | HIGH |
| ------------- | --- | ------ | ---- |
| Attack        | 150 | 200    | 255  |
| Search        | 80  | 120    | 180  |
| Turn Moderate | 20  | 25     | 40   |
| Turn Gentle   | 10  | 15     | 25   |

These presets are selectable from the SPEED menu screen.

---

### Robot State Variables

```cpp
class Robot {
    RobotMode currentMode;      // MODE_MENU, MODE_RUNNING, MODE_PAUSED
    int currentMenuScreen;      // 0-4: which menu screen displayed
    bool paused;                // true = motors stopped, sensors active
    int currentSpeedLevel;      // 0=LOW, 1=MEDIUM, 2=HIGH
    SpeedConfig speedConfig;    // Current speed values
};
```

### Mode Transitions

- **RUNNING → PAUSED:** Single press button
- **PAUSED → RUNNING:** Single press button
- **Any → MENU:** Long press button (>1000ms)
- **MENU → RUNNING:** Long press button from menu
- **In MENU:** Double press = cycle through 5 screens

---

## 📊 System Timing

| Component         | Frequency          | Period   |
| ----------------- | ------------------ | -------- |
| Main Loop         | ~200 Hz            | 5ms      |
| Sensor Reads      | 200 Hz             | 5ms      |
| Behavior Update   | 200 Hz             | 5ms      |
| Display Refresh   | ~33 Hz (throttled) | 30ms     |
| Button Update     | 200 Hz             | 5ms      |
| Gesture Detection | Real-time          | Variable |

---

## 🔗 Data Flow Summary

```mermaid
graph TB
    PIO["PlatformIO<br/>Pico Framework"] --> SETUP["setup():<br/>Arduino Entry"]
    SETUP --> ROBOT_SETUP["robot.setup()"]

    ROBOT_SETUP --> MOTOR_INIT["Motor::setup()"]
    ROBOT_SETUP --> DISP_INIT["Display::setup()"]
    ROBOT_SETUP --> IR_INIT["IRSensors::setup()"]
    ROBOT_SETUP --> BUTTON_INIT["ButtonManager::setup()"]

    LOOP["loop():<br/>Arduino Loop"] --> UPDATE["robot.update()"]
    UPDATE --> SENSOR_READ["IR/QTR Read"]
    SENSOR_READ --> BEHAVIOR["updateBehavior()"]
    BEHAVIOR --> MOTOR_CMD["Motor Commands"]

    LOOP --> BUTTON_READ["ButtonManager::update()"]
    BUTTON_READ --> GESTURE["getGesture()"]
    GESTURE --> BUTTON_HANDLER["robot.handleButtonGesture()"]
    BUTTON_HANDLER --> MODE_SWITCH["Switch Mode/Menu"]

    UPDATE --> DISPLAY_CHECK["shouldUpdate()?"]
    DISPLAY_CHECK --> DISPLAY["Display Current State"]

    LOOP --> DELAY["delay(5ms)"]
    DELAY --> LOOP

    style PIO fill:#FF6B6B,color:#fff
    style SETUP fill:#FF6B6B,color:#fff
    style LOOP fill:#FF6B6B,color:#fff
    style MOTOR_CMD fill:#4ECDC4,color:#fff
    style DISPLAY fill:#FFE66D,color:#333
```

---

## End of Architecture Documentation

For implementation details, see individual `.cpp` source files.
For hardware setup and datasheets, see `docs/datasheets/`
