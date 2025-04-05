#ifndef MECANUMCONTROL_H
#define MECANUMCONTROL_H

#include <Arduino.h>
#include <Adafruit_MCP4728.h>

class MecanumControl {
public:
  // Constructor
  MecanumControl(int lfDirPin, int rfDirPin, int lrDirPin, int rrDirPin,
                 int lfEnablePin, int rfEnablePin, int lrEnablePin, int rrEnablePin)
      : leftFrontDirPin(lfDirPin), rightFrontDirPin(rfDirPin),
        leftRearDirPin(lrDirPin), rightRearDirPin(rrDirPin),
        leftFrontEnablePin(lfEnablePin), rightFrontEnablePin(rfEnablePin),
        leftRearEnablePin(lrEnablePin), rightRearEnablePin(rrEnablePin) {}

  // Initialize the DAC and motor pins
  bool initialize() {
    if (!dac.begin()) {
      return false; // DAC initialization failed
    }
    resetDACOutputs();

    // Set direction pins as outputs
    pinMode(leftFrontDirPin, OUTPUT);
    pinMode(rightFrontDirPin, OUTPUT);
    pinMode(leftRearDirPin, OUTPUT);
    pinMode(rightRearDirPin, OUTPUT);

    // Set enable pins as outputs
    pinMode(leftFrontEnablePin, OUTPUT);
    pinMode(rightFrontEnablePin, OUTPUT);
    pinMode(leftRearEnablePin, OUTPUT);
    pinMode(rightRearEnablePin, OUTPUT);

    // Set enable pins LOW
    disableMotors();

    return true;
  }

  // Reset all DAC outputs to 0
  void resetDACOutputs() {
    dac.setChannelValue(MCP4728_CHANNEL_A, 0);
    dac.setChannelValue(MCP4728_CHANNEL_B, 0);
    dac.setChannelValue(MCP4728_CHANNEL_C, 0);
    dac.setChannelValue(MCP4728_CHANNEL_D, 0);
  }

  // Enable motors
  void enableMotors() {
    digitalWrite(leftFrontEnablePin, HIGH);
    digitalWrite(rightFrontEnablePin, HIGH);
    digitalWrite(leftRearEnablePin, HIGH);
    digitalWrite(rightRearEnablePin, HIGH);
  }

  // Disable motors
  void disableMotors() {
    digitalWrite(leftFrontEnablePin, LOW);
    digitalWrite(rightFrontEnablePin, LOW);
    digitalWrite(leftRearEnablePin, LOW);
    digitalWrite(rightRearEnablePin, LOW);
  }

  // Calculate motor powers and set motor directions and DAC outputs
  void drive(float x, float y, float turn) {
    calculateMotorPowers(x, y, turn);

    // Set motor directions and DAC outputs
    setMotor(leftFrontDirPin, leftFrontPower, MCP4728_CHANNEL_A);
    setMotor(rightFrontDirPin, rightFrontPower, MCP4728_CHANNEL_B);
    setMotor(leftRearDirPin, leftRearPower, MCP4728_CHANNEL_C);
    setMotor(rightRearDirPin, rightRearPower, MCP4728_CHANNEL_D);
  }

  // Getter methods for motor power values
  float getLeftFront() const { return leftFrontPower; }
  float getRightFront() const { return rightFrontPower; }
  float getLeftRear() const { return leftRearPower; }
  float getRightRear() const { return rightRearPower; }

private:
  Adafruit_MCP4728 dac;

  // Motor pins
  int leftFrontDirPin, rightFrontDirPin, leftRearDirPin, rightRearDirPin;
  int leftFrontEnablePin, rightFrontEnablePin, leftRearEnablePin, rightRearEnablePin;

  // Motor power values
  float leftFrontPower = 0, rightFrontPower = 0, leftRearPower = 0, rightRearPower = 0;

  // Calculate motor powers based on joystick inputs
  void calculateMotorPowers(float x, float y, float turn) {
    leftFrontPower = y + x + turn;
    rightFrontPower = y - x - turn;
    leftRearPower = y - x + turn;
    rightRearPower = y + x - turn;

    // Normalize motor powers to the range [-1, 1]
    float maxPower = max(max(abs(leftFrontPower), abs(rightFrontPower)),
                         max(abs(leftRearPower), abs(rightRearPower)));
    if (maxPower > 1) {
      leftFrontPower /= maxPower;
      rightFrontPower /= maxPower;
      leftRearPower /= maxPower;
      rightRearPower /= maxPower;
    }
  }

  // Set motor direction and output analog voltage
  void setMotor(int directionPin, float motorValue, MCP4728_channel_t channel) {
    // Scale [-1, 1] to [0, 4095]
    int dacValue = abs(motorValue * 4095);
    dacValue = constrain(dacValue, 0, 4095);

    // Set direction: HIGH = CCW, LOW = CW
    digitalWrite(directionPin, motorValue >= 0 ? LOW : HIGH);

    // Set the corresponding MCP4728 channel output
    dac.setChannelValue(channel, dacValue);
  }
};

#endif // MECANUMCONTROL_H