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

  // Check if motors are enabled
  bool areMotorsEnabled();

  // Move the robot
  void move(float x, float y, float turn);

  // Set motor RPM and direction
  void setMotorRPM(const String &motor, int rpm, bool forward);

  // Getter methods for motor power values
  float getFrontLeft() const;
  float getFrontRight() const;
  float getRearLeft() const;
  float getRearRight() const;

  // Getter for maximum RPM
  int getMaxRPM() const;

  // Set the maximum RPM
  void setMaxRPM(int maxRPM);

  // Set motor direction and output analog voltage
  void setMotor(int directionPin, float motorValue, MCP4728_channel_t channel);

  // Individual motor enable/disable functions
  void enableFrontLeftMotor();
  void disableFrontLeftMotor();
  void enableFrontRightMotor();
  void disableFrontRightMotor();
  void enableRearLeftMotor();
  void disableRearLeftMotor();
  void enableRearRightMotor();
  void disableRearRightMotor();

private:
  Adafruit_MCP4728 dac;

  // Motor pins
  const int frontLeftDirPin = 30;
  const int frontRightDirPin = 32;
  const int rearLeftDirPin = 36;
  const int rearRightDirPin = 34;

  const int frontLeftEnablePin = 42;
  const int frontRightEnablePin = 44;
  const int rearLeftEnablePin = 48;
  const int rearRightEnablePin = 46;

  // Motor power values
  float frontLeftPower = 0, frontRightPower = 0, rearLeftPower = 0, rearRightPower = 0;

  // Maximum RPM (default is 50)
  int maxRPM = 75;

  // Tracks the state of the motors
  bool motorsEnabled;

  // Helper method to set the state of all motor enable pins
  void setMotorEnableState(bool state);

  // Calculate motor powers based on joystick inputs
  void calculateMotorPowers(float x, float y, float turn);
};

#endif // MECANUMDRIVE_H