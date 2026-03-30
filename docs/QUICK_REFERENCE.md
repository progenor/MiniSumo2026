# MiniSumo2026 Quick Reference Guide

A visual quick-reference for all important concepts, commands, and behaviors.

---

## 🎮 Button Gesture Quick Reference

```
┌─────────────────────────────────────────────────────┐
│          HOW TO USE THE BUTTON (GPIO 15)            │
├─────────────────────────────────────────────────────┤
│                                                     │
│  QUICK PRESS                                        │
│  ━━━━━━━━━━━━                                       │
│  Press and release immediately                      │
│  └─→ PAUSES robot (motors off)                      │
│  └─→ Press again RESUMES robot                      │
│                                                     │
│  DOUBLE PRESS                                       │
│  ━━━━━━━━━━━━━                                      │
│  Press, release, press again within 400ms           │
│  └─→ IN MENU: Go to next screen                     │
│  └─→ IN RUNNING: Shows next menu screen (doesn't  │
│                   pause robot, just updates display)
│                                                     │
│  LONG PRESS                                         │
│  ━━━━━━━━━━━                                        │
│  Hold button for 1+ second                          │
│  └─→ TOGGLE MENU MODE (enter/exit)                 │
│  └─→ Display shows menu screens                     │
│                                                     │
└─────────────────────────────────────────────────────┘
```

---

## 🏃 Robot Operating States

```
                    STARTUP
                      ↓
                  [RUNNING]  ←──── Button: Single
                 /         \        Long Press: Stay
               ↙            ↘
          [PAUSED]        [MENU]
             ↓              ↓
          Motors motors    Cycle
          disabled active  5 screens

KEY:
[RUNNING]  = IR sensors control motors
[PAUSED]   = Sensors read, but motors frozen
[MENU]     = Display menus, adjust settings
```

---

## 🎯 IR Sensor Decision Tree

```
Read 3 IR Sensors: LEFT, CENTER, RIGHT
            │
            ↓
    ╔═══════════════════════════════════╗
    ║ What sensors are detecting? (1=Yes)║
    ╚═══════════════════════════════════╝
    │
    ├─→ [1,1,1] or [0,1,0] → ATTACK FORWARD ════════════════✓
    │   "Opponent straight ahead!"
    │   → Both motors FULL SPEED (attack_speed)
    │
    ├─→ [1,1,0] → TURN LEFT + ATTACK ════════════════════════✓
    │   "Opponent on left side!"
    │   → Left slower, Right faster
    │   → Turn left while advancing
    │
    ├─→ [0,1,1] → TURN RIGHT + ATTACK ════════════════════════✓
    │   "Opponent on right side!"
    │   → Right slower, Left faster
    │   → Turn right while advancing
    │
    ├─→ [1,0,0] → GENTLE LEFT TURN ═════════════════════════✓
    │   "Far left detection only"
    │   → Advance + slight left
    │
    ├─→ [0,0,1] → GENTLE RIGHT TURN ────────────────────────✓
    │   "Far right detection only"
    │   → Advance + slight right
    │
    └─→ [0,0,0] → SEARCH MODE ──────────────────────────────✓
        "No opponent detected!"
        → Spin in circle at search_speed
        → Increase detection chances
```

---

## ⚙️ Motor Speed Lookup

### Speed Levels (from Menu)

```
┌──────────────────────────────────────────────────┐
│  SPEED LEVEL SELECTOR (Menu Screen 1)            │
├──────────────────────────────────────────────────┤
│                                                  │
│  Double-press to cycle:                          │
│                                                  │
│  [LOW] ↔ [MEDIUM] ↔ [HIGH] ↔ [LOW] ...          │
│                                                  │
│  Selected level applies IMMEDIATELY              │
│  to running robot!                               │
│                                                  │
└──────────────────────────────────────────────────┘
```

### Power Output Comparison

```
LOW:
┌──────────────────────┐
│ Attack:    150/255   │ ████████░░ 59% power
│ Search:     80/255   │ ████░░░░░░ 31% power
│ Turn Mod:   20/255   │ █░░░░░░░░░  8% power
│ Turn Gen:   10/255   │ ░░░░░░░░░░  4% power
└──────────────────────┘

MEDIUM:
┌──────────────────────┐
│ Attack:    200/255   │ ███████░░░ 78% power
│ Search:    120/255   │ ███████░░░ 47% power
│ Turn Mod:   25/255   │ █░░░░░░░░░  9% power
│ Turn Gen:   15/255   │ █░░░░░░░░░  6% power
└──────────────────────┘

HIGH:
┌──────────────────────┐
│ Attack:    255/255   │ ██████████ 100% power
│ Search:    180/255   │ ████████░░ 71% power
│ Turn Mod:   40/255   │ ██░░░░░░░░ 16% power
│ Turn Gen:   25/255   │ █░░░░░░░░░ 10% power
└──────────────────────┘
```

---

## 🎮 Menu System Map

```
┌─────────────────────────────────────────────────────┐
│              5 MENU SCREENS - CYCLE WITH DOUBLE PRESS │
├─────────────────────────────────────────────────────┤
│                                                     │
│  SCREEN 0: STATUS                                  │
│  ┌─────────────────────────┐                       │
│  │ Mode:    [RUNNING]      │                       │
│  │ Paused:  [NO]           │                       │
│  │ Speed:   [MEDIUM]       │                       │
│  └─────────────────────────┘                       │
│          │ (double press)                          │
│          ↓                                         │
│  SCREEN 1: SPEED SELECTOR                          │
│  ┌─────────────────────────┐                       │
│  │ Speed Level:            │                       │
│  │ ► LOW                   │                       │
│  │   MEDIUM                │                       │
│  │   HIGH                  │                       │
│  │ (Double to select)      │                       │
│  └─────────────────────────┘                       │
│          │ (double press)                          │
│          ↓                                         │
│  SCREEN 2: CURRENT DRAW                            │
│  ┌─────────────────────────┐                       │
│  │ Motor A: 450 mA         │                       │
│  │ Motor B: 480 mA         │                       │
│  │                         │                       │
│  │ (Live values)           │                       │
│  └─────────────────────────┘                       │
│          │ (double press)                          │
│          ↓                                         │
│  SCREEN 3: PEAK CURRENT                            │
│  ┌─────────────────────────┐                       │
│  │ Peak Motor A: 980 mA    │                       │
│  │ Peak Motor B: 1020 mA   │                       │
│  │ Total Peak: 2000 mA     │                       │
│  │                         │                       │
│  │ (Max since startup)     │                       │
│  └─────────────────────────┘                       │
│          │ (double press)                          │
│          ↓                                         │
│  SCREEN 4: IR SENSOR READINGS                      │
│  ┌─────────────────────────┐                       │
│  │ Left:   █               │                       │
│  │ Center: ░               │                       │
│  │ Right:  ░               │                       │
│  │                         │                       │
│  │ (Bar graphs show active)│                       │
│  └─────────────────────────┘                       │
│          │ (double press)                          │
│          └─→ loops back to SCREEN 0                │
│                                                     │
│  EXIT MENU:                                        │
│  └─→ Long press button = exit to RUNNING           │
│                                                     │
└─────────────────────────────────────────────────────┘
```

---

## 🔧 Pin Configuration Quick Reference

```
RASPBERRY PI PICO PIN MAP
─────────────────────────

       ┌─────────────────────┐
       │  RP2040             │
       │  on Pico Board      │
       └─────────────────────┘

MOTORS:
    PWM_A1 = GPIO 8   (Motor A Forward)
    PWM_A2 = GPIO 9   (Motor A Backward)
    PWM_B1 = GPIO 20  (Motor B Forward)
    PWM_B2 = GPIO 21  (Motor B Backward)
    N_SLEEP = GPIO 22 (Motor Driver Enable)

SENSORS - IR:
    IR_LEFT = GPIO 12
    IR_CENTER = GPIO 13
    IR_RIGHT = GPIO 14

SENSORS - QTR LINE:
    QTR_LEFT = GPIO 2
    QTR_RIGHT = GPIO 3 ⚠️ SHARES WITH BUZZER

AUDIO:
    BUZZER = GPIO 3 ⚠️ SHARED (currently has priority)

I2C DISPLAY:
    SDA = GPIO 4 (Display data)
    SCL = GPIO 5 (Display clock)

CURRENT SENSING:
    CURRENT_A = GPIO 26 (Motor A, analog ADC2)
    CURRENT_B = GPIO 27 (Motor B, analog ADC3)

INPUT:
    BUTTON = GPIO 15

AVAILABLE PINS:
    GPIO 0, 1, 6, 7, 10, 11, 16, 17, 18, 19, 23, 24, 25, 28, 29
```

---

## 📊 System Timing

```
MAIN LOOP FREQUENCY: 200 Hz (5ms per cycle)

┌──────────┐
│iteration │
│#1        │
├──────────┤
│Time: 0ms │  ┐ 5ms
│  │       │  │
│  ├─ Read IR sensors
│  │       │  │
│  ├─ Decision logic
│  │       │  │
│  ├─ Command motors
│  │       │  │
│  ├─ delay(5ms)
│  │       │  │
└──────────┘  ┘
│
├──────────┐
│iteration │
│#2        │
├──────────┤
│Time: 5ms │  ┐ 5ms
│  │       │  │
│  ├─ Read IR sensors
│  │       │  │
│  ├─ Check: display update needed?
│  │  └─→ If ≥30ms since last: redraw (throttled)
│  │       │  │
│  └─ delay(5ms)
│       │  ┘
└──────────┘

DISPLAY REFRESH: 30ms throttle (~33 FPS max)
BUTTON UPDATE: 200 Hz (every 5ms)
SENSOR READ: 200 Hz (every 5ms)

TIMING CONSTANTS:
├─ DEBOUNCE: 20ms (buttons settle)
├─ LONG PRESS: 1000ms (>1 sec = long press)
├─ DOUBLE PRESS WINDOW: 400ms (two presses within this window)
└─ DISPLAY REFRESH: 30ms (throttle OLED writes)
```

---

## 💪 Motor Movement Combinations

```
TWO DC MOTORS - LEFT and RIGHT

Direction Control Matrix:

              MOTOR A (LEFT)
               ↙    ↖
              FWD  REV
         ┌────────────────┐
MOTOR B  │ FWD  REV  STOP │
    │    ├────────────────┤
   FWD   │F→  L←  Rt→     │
         │FWD FWD SEL     │
    │    ├────────────────┤
   REV   │L← B↙  Rt↗      │
         │BACK BACK SEL   │
    │    ├────────────────┤
   STOP  │F↑ L↙ ·STOP·    │
         │FWD TURN       │
         └────────────────┘

MEANINGS:
┌────────────────────────────────────┐
│ F→   = Forward                     │
│ B↙   = Backward                    │
│ L←   = Spin Left (counter-clock)  │
│ Rt→  = Spin Right (clockwise)     │
│ FWD  = Both forward (selected area)│
│ TURN = Turn in place              │
│ STOP = Motors off                 │
└────────────────────────────────────┘

ACTUAL MOTOR COMMANDS USED:

forward(pwm)   = Both FWD at same speed
backward(pwm)  = Both REV at same speed
left(pwm)      = Motor B FWD, Motor A REV (spin left)
right(pwm)     = Motor A FWD, Motor B REV (spin right)
stop()         = Both motors off (0 PWM)
```

---

## 🔍 IR Sensor Detection Range

```
          ROBOT FRONT
             ↑
       ╔═════╩═════╗
       │  IR LENS  │
       └─────┬─────┘
       ╱  │  │  │  ╲
      ╱   │  │  │   ╲
     ╱    │  │  │    ╲
    ╱  L  │  C  │  R  ╲
   ╱1000mm     1000mm  ╲
  ╱                      ╲
 ╱                        ╲  ~10cm detection
 ────────────────────────────── range
           ARENA


DETECTION SPECS:
├─ Maximum Range: ~10cm
├─ Detection: HIGH (1) when reflector in range
├─ Non-detect: LOW (0) when no reflector
├─ Active: Infrared LED reflectance
├─ Mounting: Pico breakout pins 12, 13, 14
└─ Scan Rate: 200 Hz (every 5ms)

IR LENS ARRANGEMENT:
  [LEFT]  [CENTER]  [RIGHT]
  GPIO12  GPIO13    GPIO14
   ←→        ↑         ←→
   10°      5°         10°
  offset  straight  offset
```

---

## 📈 Current Monitoring

```
MOTOR CURRENT MEASUREMENT

┌────────────────────────────────────────┐
│     SENSING SYSTEM FLOW                │
├────────────────────────────────────────┤
│                                        │
│ Motor A                                │
│ (Left)                                 │
│    ↓                                   │
│ Current Sensor                         │
│ (GPIO 26 - ADC)                        │
│    ↓                                   │
│ Analog Read (0-4095 bits)              │
│    ↓                                   │
│ Convert to Amps                        │
│    ↓                                   │
│ Alpha Filter (smooth)                  │
│ (α = 0.97 = strong smoothing)         │
│    ↓                                   │
│ Track Peak Value                       │
│    ↓                                   │
│ Display on Menu                        │
│                                        │
│ (Same for Motor B on GPIO 27 - ADC)   │
│                                        │
└────────────────────────────────────────┘

VALUES DISPLAYED:
├─ Real-time Current: _____ mA (now)
├─ Filtered Current: _____ mA (smoothed)
└─ Peak Current: _____ mA (highest ever)

WHAT IT MEANS:
├─ Normal Attack: 600-800 mA per motor
├─ Search Spin: 200-400 mA per motor
├─ Stalled Motor: >1000 mA sustained
├─ Low Battery: Attack current < 500 mA
└─ High Friction: Current > normal for speed
```

---

## 🚀 Startup Sequence

```
POWER ON
   ↓
setup() - Arduino
   ↓
robot.setup()
   ├─ Motor::setup()
   │  └─ Initialize DRV8243 driver
   │     └─ Set motor pins OUTPUT
   │        └─ Disable motor power (N_SLEEP = LOW)
   │
   ├─ Display::setup()
   │  └─ Initialize SSD1306 OLED
   │     └─ Draw loading screen with logo
   │
   ├─ IRSensors::setup()
   │  └─ Set IR pins INPUT
   │
   ├─ ButtonManager::setup()
   │  └─ Configure GPIO 15 INPUT_PULLUP
   │
   └─ QTRSensors::setup()
      └─ Set line sensor pins INPUT
   │
   ↓
READY - Waiting for button
   │
   └─── BUTTON PRESS →
         │
         ↓
      loop() - Arduino continuous 200Hz
         │
         ├─ robot.update()
         │  ├─ Read all sensors
         │  ├─ Decision logic
         │  └─ Command motors
         │
         ├─ ButtonManager::update()
         │  ├─ Check button state
         │  └─ Detect gesture
         │
         └─ delay(5ms)

   ↓
RUNNING - IR responsive mode begins!
```

---

## 🎯 Attack Flowchart (Once Robot Detects Opponent)

```
RUNNING MODE
    │
    ↓
Every 5ms:
    │
    ├─ Read IR sensors
    │  └─ Sample Left, Center, Right
    │
    ├─ Compare to decision tree
    │  ├─ Is Center detected? YES → ATTACK
    │  ├─ Are Left+Center? YES → LEFT ATTACK
    │  ├─ Are Right+Center? YES → RIGHT ATTACK
    │  ├─ Is Left only? YES → HUNT LEFT
    │  ├─ Is Right only? YES → HUNT RIGHT
    │  └─ Nothing detected? → SEARCH SPIN
    │
    ├─ Send motor command
    │  ├─ Motor A: ±pwm value (FWD/REV)
    │  ├─ Motor B: ±pwm value (FWD/REV)
    │  └─ PWM value = attack_speed, search_speed, turn_speed
    │
    ├─ Every 30ms (if time):
    │  └─ Update display
    │     ├─ Show IR bar graphs
    │     ├─ Show current mode
    │     └─ Show speed level
    │
    └─ Next cycle (5ms later)

CYCLE REPEATS ~200 TIMES PER SECOND
```

---

## 🆘 Performance Checklist

```
┌──────────────────────────────────────────────┐
│  ROBOT HEALTH CHECK                          │
├──────────────────────────────────────────────┤
│                                              │
│ ✓ IR SENSORS WORKING?                        │
│   └─ Menu Screen 4 (IR Readings)             │
│      Should show bars when opponent near     │
│                                              │
│ ✓ BUTTON RESPONDING?                         │
│   └─ Single press → Pause/Resume             │
│      Long press → Enter Menu                 │
│      Double press → Cycle menus              │
│                                              │
│ ✓ MOTORS MOVING?                             │
│   └─ Menu Screen 2 (Current Draw)            │
│      Should show 300+ mA when moving         │
│      Both motors should have similar current │
│                                              │
│ ✓ DISPLAY CLEAR?                             │
│   └─ All menus readable                      │
│      No flickering or corruption             │
│                                              │
│ ✓ BATTERY VOLTAGE OK?                        │
│   └─ Attack speed should be full (255)       │
│      If lower, battery may be weak           │
│                                              │
│ ✓ ATTACK POWER ADEQUATE?                     │
│   └─ When set to HIGH speed:                 │
│      Motor current > 700mA                   │
│      Robot can push hard                     │
│                                              │
│ ✓ DRIFT/UNBALANCED?                          │
│   └─ When moving forward:                    │
│      Robot goes straight (not left/right)    │
│      Both motor currents within 50mA         │
│                                              │
└──────────────────────────────────────────────┘
```

---

## 📱 Display Content Reference

### RUNNING Mode Display

```
┌─────────────────────────────────────┐
│  ║ ║ ║  IR SENSORS                  │
│  ║ ║ ║  L  C  R                      │
│  █ ░ ░  (Active sensors show bars)  │
│  ─ ─ █                              │
│                                     │
│  Speed: MEDIUM                      │
│  Mode:  RUNNING                     │
│                                     │
│  [Press btn to pause]               │
│  [Long press for menu]              │
│                                     │
└─────────────────────────────────────┘
```

### PAUSED Mode Display

```
┌─────────────────────────────────────┐
│  ║ ║ ║  IR SENSORS                  │
│  ║ ║ ║  L  C  R                      │
│  ░ █ ░  (Frozen at pause time)      │
│  ─ ─ ─                              │
│                                     │
│  Speed: MEDIUM                      │
│  Mode:  PAUSED                      │
│                                     │
│  [Motors disabled]                  │
│  [Press btn to resume]              │
│                                     │
└─────────────────────────────────────┘
```

### MENU Mode Display

```
┌─────────────────────────────────────┐
│  MENU MODE                          │
│                                     │
│  Status Screen                      │
│  ─────────────────                  │
│  Mode:    MENU                      │
│  Paused:  NO                        │
│  Speed:   MEDIUM                    │
│                                     │
│  [Double to next screen]            │
│  [Long press to exit]               │
│                                     │
└─────────────────────────────────────┘
```

---

## 📞 Quick Troubleshooting Guide

```
PROBLEM                    SOLUTION
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
Robot doesn't move         └─ Check: RUNNING mode active?
                           └─ Check: Battery connected?
                           └─ Check: Motor current > 0?

Motors spin at            └─ Check: Motor A vs B current
different speeds          └─ Check: Wheel/load difference

IR sensors not            └─ Check: Menu Screen 4
detecting something       └─ Check: 10cm range requirement
                          └─ Check: Opponent reflective?

Display blank             └─ Check: I2C address 0x3C
                          └─ Check: SDA/SCL pins GPIO 4/5
                          └─ Try: Re-upload firmware

Button not working        └─ Tap firmly, hold 1+ sec for long
                          └─ Check: Pin GPIO 15
                          └─ Try: Battery voltage check

Robot moving too          └─ Change speed level (Menu Screen 1)
fast / too slow          └─ Adjust attack_speed in defines.h

Unbalanced/drifting      └─ Check: Motor current balance
                          └─ Check: Wheel wear
                          └─ Adjust: Turn speed constants
```

---

**Quick Ref v1.0** | MiniSumo2026 | 2026-03-30
