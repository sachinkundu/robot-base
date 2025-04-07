#ifndef MECANUMDRIVE_H
#define MECANUMDRIVE_H

#include <Arduino.h>
#include <Adafruit_MCP4728.h>

class MecanumDrive {
public:
  MecanumDrive();

  // Initialize the DAC and motor pins
  bool initialize();

  // Reset all DAC outputs to 0
  void resetDACOutputs();

  // Enable motors
  void enableMotors();

  // Disable motors
  void disableMotors();

  // Move the robot
  void move(float x, float y, float turn);

  // Set motor RPM and direction
  void setMotorRPM(const String &motor, int rpm, bool forward);

  // Getter methods for motor power values
  float getLeftFront() const;
  float getRightFront() const;
  float getLeftRear() const;
  float getRightRear() const;
  
  // Getter for maximum RPM
  int getMaxRPM() const;

  // Set the maximum RPM
  void setMaxRPM(int maxRPM);

  // Set motor direction and output analog voltage
  void setMotor(int directionPin, float motorValue, MCP4728_channel_t channel);

  // Individual motor enable/disable functions
  void enableLeftFrontMotor();
  void disableLeftFrontMotor();
  void enableRightFrontMotor();
  void disableRightFrontMotor();
  void enableLeftRearMotor();
  void disableLeftRearMotor();
  void enableRightRearMotor();
  void disableRightRearMotor();

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

  // Maximum RPM (default is 50)
  int maxRPM = 75;

  // Helper method to set the state of all motor enable pins
  void setMotorEnableState(bool state);

  // Calculate motor powers based on joystick inputs
  void calculateMotorPowers(float x, float y, float turn);

};

#endif // MECANUMDRIVE_H