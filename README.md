# HIL Quadrotor Simulator

A hardware-in-the-loop (HIL) testbed for quadrotor flight controller
development. Real flight-controller firmware runs on an ATmega328,
exchanging sensor and motor-command data over UART with a physics-based
quadrotor simulation running on a host PC.


## Features

- **13-state nonlinear quadrotor dynamics** — 6-DOF rigid body (position,
  velocity, quaternion attitude, angular velocity), integrated with RK4.
- **Per-motor electrical + mechanical dynamics** First order approximation
  of a BLDC motor.
- **Aerodynamic drag** (Not yet implemented...) (linear translational + rotational damping) so
  the vehicle reaches terminal velocity instead of accelerating forever.
- **Custom binary UART protocol** — length-prefixed, checksummed frames,
  self-resyncing after a corrupted or dropped byte.
- **Real-time on both ends** — fixed-timestep physics loop on the host
  (`clock_nanosleep` with absolute deadlines), fixed-rate control loop on
  the MCU (Timer1 CTC interrupt).
- **Optional sensor noise/bias model** (TODO: Add more sensors) for testing controller robustness
  against realistic IMU and gyro data.
- **Clean controller interface** — `controller.c` has zero dependency on
  UART/timer/register code, so control logic can be modified or
  unit-tested independently of the hardware layer.

## Project structure

```
common/
  protocol.h            # shared frame format + struct definitions
                         # (single source of truth for both builds)
firmware/
  common/
    ring_buffer.c/h      # generic byte ring buffer (no hardware deps)
    protocol.c           # MCU-side frame parser/sender
    controller.c/h       # control law -- pure function, no hardware deps
  targets/
    atmega328/
      main.c             # wires uart/timer/pwm/controller together
      Makefile
      drivers/
        uart.c/h           # interrupt-driven UART driver
        timer.c/h          # Timer1 CTC (control loop tick)
        gpio.c /h          # I/O pin drivers
        adc.c / h          # ADC init and reading (This was implemented for testing mainly... Might be used in the future) 
hil/
  main.c                 # host-side physics loop + simulation entry point
  src/
    protocol.c           # host-side frame parser/sender
    serial.c /.h         # Serial interface / UDP config for Python Viz (Not yet commited).
    solver.c / h         # fixed-step ODE integrators (Euler/RK4) with Quadcopter model
```

## Building

You can run the Makefiles individually or run ./build.sh from the (this projects) root directory to flash and run the simulator.

## Protocol

Binary frames, both directions:

```
[START_BYTE] [LENGTH] [PAYLOAD...] [XOR_CHECKSUM]
```

| Field | Notes |
|---|---|
| `START_BYTE` | `0xA5` (host→MCU sensor data) or `0x5A` (MCU→host motor command) — direction-specific so a receiver can tell immediately if it's out of sync |
| `LENGTH` | Must match `sizeof(State_t)` or `sizeof(MotorCommand_t)`; mismatches drop the frame and resync on the next start byte |
| `PAYLOAD` | Packed struct bytes (`__attribute__((packed))`, `float`-only, no `double`, so layout is identical on both AVR and x86 builds) |
| `XOR_CHECKSUM` | XOR of all payload bytes; mismatch drops the frame |

A single dropped or corrupted byte costs at most one bad frame — the
parser always resyncs at the next valid start byte.

## Hardware wiring

| Signal | ATmega328 pin | Notes |
|---|---|---|
| UART TX/RX | Hardware USART0 | Via USB-serial adapter to host |
| Motor 1 PWM | `OC0A` (PD6) | Timer0, Fast PWM |
| Motor 2 PWM | `OC0B` (PD5) | Timer0, Fast PWM |
| Motor 3 PWM | `OC2A` (PB3) | Timer2, Fast PWM |
| Motor 4 PWM | `OC2B` (PD3) | Timer2, Fast PWM |

Timer1 is reserved for the 500Hz control-loop tick (CTC mode) and is
not available for PWM output.

## TODO:
- [ ] Add actual control.
- [ ] Higher fidelity model of quadcopter.

