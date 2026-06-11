# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

ESP32-based differential drive robot for WRO Robomission Senior. Uses pure pursuit path tracking, PID control, encoder odometry, and IMU sensor fusion. The robot has 4 subsystems: drive base, toolhead gantry (X/Z axes), wheels, and mill.

## Build & Run

This is an Arduino IDE project (no platformio.ini). Compile and upload from the Arduino IDE targeting ESP32 Dev Module.

- **Toggle simulation mode**: Uncomment `#define SIMULATION` at the top of `Cody.ino`
- **Real hardware mode** (default): Comments out SIMULATION, uses `SensorDataProvider` and `RobotHardwareProvider`
- **Serial baud rate**: 115200 (defined in `Cody.ino`)
- **Control loop rate**: 50 Hz (`#define HZ 50.0` in `Cody.h`)

## Architecture

### Data pipeline (every control cycle)
```
IDataProvider::getData() → SensorData
  → Fusion::getData(SensorData) → FusionData   // encoder odometry + IMU fusion
    → Navigation::getData(FusionData, speed) → MotorData  // PID controllers
      → IHardwareProvider::move(NavigationData)  // set motor PWMs
```

### Key files

- **Cody.ino** — Entry point. `setup()` initializes hardware, then loops waiting for button press → `Program::go()`
- **Cody.h/.cpp** — Static facade API. Each subsystem has *Async() methods returning `Task*` (FreeRTOS). Uses a shared `followPathTask` with function pointers for the generic pure pursuit loop
- **Program.h** — Mission program. `go()` calls Cody methods to execute competition tasks
- **Fusion.h/.cpp** — Sensor fusion: Mahony filter (IMU orientation) + differential drive odometry. Tracks 4 positions (robot, toolhead, wheels, mill) and voltage/color
- **Navigation.h/.cpp** — Converts fused state + target into motor commands. 6 PID controllers (orientation, distance, xAxis, zAxis, wheels, mill)
- **Pursuit.h** — Pure pursuit: circle-line intersection to find lookahead point on a polyline path
- **PID.h** — PID controller with integral windup clamping + output alpha smoothing
- **Task.h** — FreeRTOS task wrapper. Runs on core 1 with 10K stack. `await()` blocks until task completes
- **BMS.h** — Background battery monitor task. Beeps buzzer when voltage ≤ 3.4V

### Data types

- **SensorData** — Raw sensor readings (encoder pulses, IMU, BMS, limit switches, color)
- **FusionData** — Fused state (orientation, 4x positions, voltage, detected color)
- **NavigationTarget** — Shared mutable target (position, steering target, deceleration distance) updated each cycle by the pure pursuit loop
- **PursuitData** — Path waypoints + lookahead distance + current line index
- **NavigationData / ToolheadData / WheelsData / MillData** — Motor command structs (MotorData: forwards bool + pwm int)

### Hardware providers (interface pattern)

- **IDataProvider** — `getData()` returns SensorData
  - `SensorDataProvider` — Real hardware: MPU9250 (I2C), ADS1115 (battery ADC), TCS34725 (color), 6x ESP32Encoder, PCF8575 GPIO (limit switches, button)
  - `SimulationDataProvider` — Reads JSON from Serial for simulation
- **IHardwareProvider** — `move()`, `moveToolhead()`, `moveWheels()`, `moveMill()`, LED/buzzer
  - `RobotHardwareProvider` — TB6612FNG motor drivers via PCF8575 GPIO direction pins + analogWrite PWM
  - `SimulationHardwareProvider` — Sends JSON commands over Serial

### Pin assignments

- **Encoders** (half-quadrature): L=26/27, R=32/33, X=34/35, Z=36/39, Wheels=12/13, Mill=14/15
- **Motor PWMs**: L=2, R=4, X=16, Z=17, Wheels=18, Mill=19
- **Motor direction** (via PCF8575 GPIO expander): pins 0-11
- **I2C**: IMU at 0x68, ADC at 0x48, PCF8575 at default address
- **LED**: GPIO 12, **Buzzer**: GPIO 5

### PID tuning (in Navigation.cpp)

- Orientation: Kp=3.5, Ki=0, Kd=0
- Distance: Kp=0.5, Ki=0, Kd=0
- All subsystem axes: Kp=1.0, Ki=0, Kd=0

### Color calibration (in Fusion.cpp)

RGB reference values for 5 colors (Black, White, Blue, Green, Yellow). Color matching uses nearest-neighbor (minimum sum of RGB differences).

## Concurrency model

- `Task` wraps `xTaskCreatePinnedToCore` (always pinned to core 1)
- Tasks run at 50 Hz with `vTaskDelay` for timing
- `Cody::followPathTask` is the generic control loop: reads sensors, fuses, computes lookahead, applies PID, moves motors — repeats until path end is reached
- `BMS::bmsTask` runs continuously at 1 Hz monitoring voltage
- The `loop()` function in Cody.ino runs on core 0 (Arduino convention) and is unused during competition runs

## Common workflows

- **Add a new path point**: `Cody::addPathPoint(x, y)` or `Cody::addToolheadPathPoint(x, z)` etc.
- **Run async and block**: `Cody::moveAsync(x, y)->await()`
- **Test in simulation**: uncomment `#define SIMULATION` in Cody.ino (software-in-the-loop with Unity simulation)
