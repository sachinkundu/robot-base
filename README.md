# Omnidirectional Mecanum Wheel Robot Base

## Overview
This project controls a robot base with Mecanum wheels using an Xbox controller. The Arduino processes joystick inputs to calculate motor power values and sends them to the motors through a DAC.

### Code Overview
- `XboxController.h`: Reads and processes Xbox controller inputs.
- `MecanumControl.h`: Calculates motor power values for Mecanum wheels.
- `RobotControl.ino`: Integrates Xbox input and motor control, sending outputs to the DAC.
