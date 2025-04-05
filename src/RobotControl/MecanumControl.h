#ifndef MECANUMCONTROL_H
#define MECANUMCONTROL_H

#include <Arduino.h>
#include <Adafruit_MCP4728.h>

class MecanumControl {
public:
  // Constructor
  MecanumControl();

  // Initialize the DAC and motor pins
  bool initialize();

  // Reset all DAC outputs to 0
  void resetDACOutputs();

  // Enable motors
  void enableMotors();

  // Disable motors
  void disableMotors();

  // Calculate motor powers and set motor directions and DAC outputs
  void drive(float x, float y, float turn);

  // Getter methods for motor power values
  float getLeftFront() const;
  float getRightFront() const;
  float getLeftRear() const;
  float getRightRear() const;

private:
  Adafruit_MCP4728 dac;

  // Motor pins
  const int leftFrontDirPin = 33;
  const int rightFrontDirPin = 32;
  const int leftRearDirPin = 30;
  const int rightRearDirPin = 31;

  const int leftFrontEnablePin = 42;
  const int rightFrontEnablePin = 43;
  const int leftRearEnablePin = 44;
  const int rightRearEnablePin = 45;

  // Motor power values
  float leftFrontPower = 0, rightFrontPower = 0, leftRearPower = 0, rightRearPower = 0;

  // Helper method to set the state of all motor enable pins
  void setMotorEnableState(bool state);

  // Calculate motor powers based on joystick inputs
  void calculateMotorPowers(float x, float y, float turn);

  // Set motor direction and output analog voltage
  void setMotor(int directionPin, float motorValue, MCP4728_channel_t channel);
};

#endif // MECANUMCONTROL_H