# MiniSumo2026 Robot 🤖

A small sumo robot built on **Raspberry Pi Pico** that autonomously detects and fights opponents in a ring. Uses infrared sensors to hunt, a menu system for tuning, and intelligent motor control for aggressive gameplay.

---

## 🎯 Quick Overview

The MiniSumo2026 is designed to:

1. **Detect opponents** using 3 infrared sensors pointing forward
2. **Attack aggressively** when an opponent is directly ahead
3. **Search intelligently** with predictable patterns when no opponent is detected
4. **Avoid falling off** the ring using line sensors (future feature)
5. **Be configured** via on-board menu and button

---

## 🚗 How the Robot Works

### The Basic Loop (happens 200 times per second)

```
1. READ SENSORS
   ↓
2. MAKE DECISION (based on what sensors see)
   ↓
3. COMMAND MOTORS (attack, search, or stop)
   ↓
4. UPDATE DISPLAY (show status)
   ↓
5. REPEAT
```

### Sensor Interpretation: The Three Eyes 👀

The robot has **3 infrared sensors** pointing forward, left-center-right:

```
           LEFT
            ↑
   ↙ ← ← ← ← → → → ↘
  FRONT VIEW -->

IR Left    IR Center    IR Right
  GPIO12     GPIO13       GPIO14
```

**What sensors detect:**

- **HIGH (1)** = Opponent detected in that direction (~10cm away)
- **LOW (0)** = No opponent in that direction

### Decision Logic: What to Do?

The robot checks all 8 possible combinations of 3 sensors and decides:

| Sensors Detect             | What Robot Does             | Purpose                     |
| -------------------------- | --------------------------- | --------------------------- |
| **All 3** (Center + Sides) | ATTACK STRAIGHT             | Opponent directly ahead     |
| **Center Only**            | ATTACK STRAIGHT             | Opponent directly ahead     |
| **Center + Left**          | SHARP LEFT TURN             | Opponent on left side       |
| **Center + Right**         | SHARP RIGHT TURN            | Opponent on right side      |
| **Left Only**              | GENTLE LEFT TURN + FORWARD  | Opponent moved to left      |
| **Right Only**             | GENTLE RIGHT TURN + FORWARD | Opponent moved to right     |
| **All 3 + Sides**          | SHARP TURN                  | Opponent very close to side |
| **None (All 0)**           | ROTATE IN PLACE             | Searching for opponent      |

### Speed Control 🏎️

The robot has **3 speed levels** that can be selected from the menu:

| Level               | Purpose              | Motor Power              |
| ------------------- | -------------------- | ------------------------ |
| **LOW** (Red)       | Practice/Debugging   | Attack: 150, Search: 80  |
| **MEDIUM** (Yellow) | Balanced Competition | Attack: 200, Search: 120 |
| **HIGH** (Green)    | Full Aggression      | Attack: 255, Search: 180 |

Each level has separate speeds for:

- **Attack Speed** - when opponent is detected
- **Search Speed** - when rotating/searching
- **Turn Speeds** - sharp vs. gentle turns

---

## 🔘 Button Control

The robot has **1 button** (GPIO 15) that controls everything:

### Button Gestures

| Gesture          | How                      | What Happens                                         |
| ---------------- | ------------------------ | ---------------------------------------------------- |
| **Single Press** | Quick press & release    | Toggle PAUSE (freeze motors but keep sensors active) |
| **Double Press** | 2 quick presses in a row | Cycle through MENU SCREENS or increase SPEED         |
| **Long Press**   | Hold for 1+ second       | Enter/Exit MENU MODE                                 |

### Debouncing ✨

The button has smart debouncing (20ms) + gesture detection timeout (400ms window) so accidental bounces don't trigger false gestures.

---

## 🎮 Operating Modes

The robot works in **3 main modes**:

### 1️⃣ RUNNING Mode (Normal Operation)

```
Button Press
     ↓
IR Sensors Read
     ↓
Decision Logic
     ↓
Motors Move
     ↓
Display Shows: IR Bars (where opponent detected?)
     ↓
Repeat every 5ms
```

**You see:** Real-time IR sensor bar graphs showing what each sensor detects

**Single Press Button:** Pause the motors

### 2️⃣ PAUSED Mode (Motors Off, Sensors Active)

```
Motors Stop
     ↓
Sensors Still Read
     ↓
Display Locked (shows current IR reading)
     ↓
Motors Don't Respond
```

**You see:** Same IR bars, but robot stays still

**Single Press Button:** Resume to RUNNING mode

### 3️⃣ MENU Mode (Configuration & Diagnostics)

Long press the button to enter the menu. Use double press to cycle through screens:

#### Menu Screens (5 Total):

**Screen 0: STATUS**

- Current Mode (MENU/RUNNING/PAUSED)
- Pause indicator
- Current Speed Level

**Screen 1: SPEED SELECTOR**

- Shows available speeds: LOW → MEDIUM → HIGH
- Double press to cycle and select
- Selected level applies immediately

**Screen 2: CURRENT DRAW**

- Real-time current for Motor A: **\_** mA
- Real-time current for Motor B: **\_** mA
- Helps diagnose electrical issues

**Screen 3: PEAK CURRENT**

- Peak current detected on Motor A
- Peak current detected on Motor B
- Total peak current (both motors)
- Shows maximum draw during operation

**Screen 4: SENSOR READINGS**

- Live bar graphs for all 3 IR sensors
- Visual confirmation sensors are working
- See exactly what the robot "sees"

**Exit Menu:** Long press button again to return to RUNNING mode

---

## ⚙️ Motor Control Details

### Motor Basics

The robot has **2 motors** (left and right):

```
       FRONT
         ↑
    [Left]  [Right]
       |      |
       ↓      ↓
     Motor    Motor
       A        B
```

### Movement Commands

| Command        | Left Motor | Right Motor | Result            |
| -------------- | ---------- | ----------- | ----------------- |
| **Forward**    | Forward    | Forward     | 👉 Straight ahead |
| **Backward**   | Backward   | Backward    | 👈 Straight back  |
| **Left Turn**  | Backward   | Forward     | ↙️ Spin left      |
| **Right Turn** | Forward    | Backward    | ↘️ Spin right     |
| **Stop**       | Off        | Off         | ⏹️ No movement    |

### Power Levels

Motor power is measured as **PWM values (0-255)**:

- **0** = No power (coasting)
- **128** = 50% power
- **255** = Full power (maximum speed)

The speed configuration determines which PWM is sent (e.g., attack_speed = 250 means 250/255 ≈ 98% power).

### Current Sensing 📊

Both motors have current sensors:

- **Real-time current:** Shows instantaneous draw
- **Filtered current:** Smoothed over time (removes noise)
- **Peak current:** Highest draw since robot started

This helps detect:

- Motor stalling (high current, no movement)
- Battery low (attack_speed below normal)
- Mechanical issues

---

## 🎯 How It Fights

### Basic Strategy

```
1. STARTUP: Motors disabled, waiting
   ↓
2. BUTTON: Single press → enter RUNNING mode
   ↓
3. SEARCH: Rotate in place, all sensors scanning
   ↓
4. DETECT: Opponent found by IR sensor!
   ↓
5. LOCK-ON: Center sensor detects, go full attack!
   ↓
6. PUSH: Drive straight forward at full speed
   ↓
7. END: Opponent falls off ring, or...
   ↓
8. BACK TO SEARCH: If opponent escapes, hunt again
```

### Attack Patterns

**Head-on (Center sensor):**

- All motors forward at ATTACK_SPEED
- Result: Straight aggressive push

**Right bias (Right + Center):**

- Left motor faster
- Right motor slower
- Result: Sharp right turn while advancing

**Left bias (Left + Center):**

- Right motor faster
- Left motor slower
- Result: Sharp left turn while advancing

**Searching (No sensors):**

- Left motor backward, right motor forward
- Spin in circles at SEARCH_SPEED
- Increases chances of detecting opponent

---

## 🖥️ Display System

### Hardware

- **Type:** 128×64 OLED (small, bright, clear)
- **Connection:** I2C (wireless-like digital communication)
- **Refresh Rate:** ~33 times per second (smooth visuals)

### What You See

In **RUNNING mode**, you see:

```
┌────────────────────────┐
│  ║ ║ ║   IR SENSORS   │
│  ║ ║ ║                 │
│  ║ ║ ║  L  C  R        │
│  █   █    █ █ █        │
│  (Bar graph showing    │
│   which sensors fired) │
│                        │
│  Speed: HIGH           │
│  Mode: RUNNING         │
└────────────────────────┘
```

In **MENU mode**, display changes to show requested information (speed, current, etc.)

---

## 📦 What's Inside (Hardware)

### Brain

- **Microcontroller:** Raspberry Pi Pico (similar to Arduino)
- **Processor:** ARM Cortex-M0+ at 125 MHz
- **Memory:** 264 KB RAM, 2 MB Flash

### Sensors

| Sensor              | Count | Job                               |
| ------------------- | ----- | --------------------------------- |
| **IR Proximity**    | 3     | Find opponent                     |
| **Line Detectors**  | 2     | Prevent ring falls (not yet used) |
| **Current Monitor** | 2     | Measure motor power               |

### Motors & Driver

- **Motor Driver:** DRV8243 (controls both motors)
- **Motors:** 2 DC motors with PWM control
- **Max Power:** 2 Amps per motor
- **Pins Used:** 4 PWM pins + 1 enable pin = 5 GPIO total

### Output

- **Display:** 128×64 OLED over I2C
- **Buzzer:** Audio feedback on GPIO 3

### Input

- **Button:** Single GPIO 15 for all control

### Power

- **Voltage:** 3.3V logic
- **Motor Supply:** Battery connector (typical 7.2V LiPo)

---

## 🔧 Hardware Pin Map

```
GPIO Pins Used:
├─ Motors
│  ├─ GPIO 8,9:   Motor A PWM control
│  ├─ GPIO 20,21: Motor B PWM control
│  └─ GPIO 22:    Motor driver enable
├─ Sensors
│  ├─ GPIO 12,13,14: IR sensors (3 total)
│  └─ GPIO 2,3:     Line sensors (2 total)
├─ Audio
│  └─ GPIO 3:       Buzzer (shares with line sensor)
├─ I2C Display
│  ├─ GPIO 4:       SDA
│  └─ GPIO 5:       SCL
├─ Power Sensing
│  ├─ GPIO 26:      Current A (analog)
│  └─ GPIO 27:      Current B (analog)
└─ Input
   └─ GPIO 15:      Button
```

---

## 🚀 Getting Started

### 1. Load Firmware

```bash
# Connect Pico to computer via USB
# Open PlatformIO in VS Code
# Click "Upload" button
platformio run --target upload --environment pico
```

### 2. Power Up

- Connect battery to robot
- Motor power will be **disabled** by default (safety)

### 3. First Run

1. Press button **once** on robot
   - Robot enters RUNNING mode
   - Motors enable
   - IR sensors start scanning
2. Place robot in arena
3. When opponent approaches:
   - IR sensors detect
   - Robot automatically attacks
   - No further input needed!

### 4. Configure Speeds

1. Long press button to enter MENU
2. Double press to navigate screens
3. Go to SPEED screen
4. Double press to cycle: LOW → MEDIUM → HIGH
5. Long press to exit menu
6. New speed applies immediately

---

## 📋 Configuration Values

All editable in `defines.h`:

```cpp
// Speed presets (PWM values 0-255)
SPEED_PRESETS[3] = {
    // LOW:    Attack=150, Search=80, TurnMod=20, TurnGen=10
    // MEDIUM: Attack=200, Search=120, TurnMod=25, TurnGen=15
    // HIGH:   Attack=255, Search=180, TurnMod=40, TurnGen=25
};

// Button timing
DEBOUNCE_MS = 20       // Ignores bounces < 20ms
LONG_PRESS_MS = 1000   // Long press > 1 second
DOUBLE_PRESS_WINDOW = 400 // Double press within 400ms

// Display
DISPLAY_REFRESH_MS = 30  // Redraw every ~30ms (~33 FPS)
```

---

## 🐛 Troubleshooting

### Robot doesn't move

- **Check:** Is it in RUNNING mode? (not PAUSED)
- **Check:** Are motors powered? (DRV8243 enable pin 22)
- **Check:** Is battery connected? (3.7V+ for motors)
- **Check:** No button is held?

### Motors run at different speeds

- **Check:** Motor A current vs B current (unbalanced load)
- **Check:** Are both motors the same type?
- **Solution:** Adjust turn PWM values to compensate

### IR sensors not detecting

- **Check:** Menu → Sensor Readings screen shows bars
- **Check:** Opponent within 10cm of sensors
- **Check:** Are IR LEDs visible (if night vision camera)?
- **Solution:** Clean sensor lenses

### Display is blank

- **Check:** Is I2C address correct? (0x3C)
- **Check:** Are SDA/SCL pins connected? (GPIO 4/5)
- **Check:** Try re-uploading firmware

### Button gestures not working

- **Check:** Is button pressed firmly?
- **Check:** Battery voltage (low voltage affects GPIO?)
- **Solution:** Test button directly in debug mode

---

## 🎓 Learning Resources

### File Structure

```
MiniSumo2026/
├─ src/          Source code (C++)
│  ├─ main.cpp   Entry point
│  ├─ robot.cpp  Core logic
│  ├─ motors.cpp Motor control
│  ├─ display.cpp Display rendering
│  ├─ ir.cpp     IR sensor reading
│  └─ button.cpp Button handling
├─ include/      Header files (.h)
├─ docs/         Documentation
│  ├─ PROJECT_ARCHITECTURE.md  (this file)
│  ├─ robot.md   Hardware details
│  └─ datasheets/ Component datasheets
└─ platformio.ini Configuration
```

### Key Files to Understand

1. **main.cpp** - Arduino setup() and loop()
2. **robot.cpp** - Decision logic (IR → Motor commands)
3. **motors.cpp** - Motor PWM control
4. **button.cpp** - Gesture detection algorithm
5. **display.cpp** - Menu rendering

---

## 📞 Contact & Support

For questions about the MiniSumo2026:

- Check `docs/robot.md` for hardware specs
- Review code comments in `.cpp` files
- Check `include/README` for API documentation
- See `docs/datasheets/` for component specs

---

**Built with ❤️ for T.F.S Robotics**

_Last Updated: 2026-03-30_
