#include "MecanumControl.h"

// Constructor
MecanumControl::MecanumControl() {}

// Initialize the DAC and motor pins
bool MecanumControl::initialize() {
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
void MecanumControl::resetDACOutputs() {
  dac.setChannelValue(MCP4728_CHANNEL_A, 0);
  dac.setChannelValue(MCP4728_CHANNEL_B, 0);
  dac.setChannelValue(MCP4728_CHANNEL_C, 0);
  dac.setChannelValue(MCP4728_CHANNEL_D, 0);
}

// Helper method to set the state of all motor enable pins
void MecanumControl::setMotorEnableState(bool state) {
  digitalWrite(leftFrontEnablePin, state ? HIGH : LOW);
  digitalWrite(rightFrontEnablePin, state ? HIGH : LOW);
  digitalWrite(leftRearEnablePin, state ? HIGH : LOW);
  digitalWrite(rightRearEnablePin, state ? HIGH : LOW);
}

// Enable motors
void MecanumControl::enableMotors() {
  setMotorEnableState(true);
}

// Disable motors
void MecanumControl::disableMotors() {
  setMotorEnableState(false);
}

// Calculate motor powers and set motor directions and DAC outputs
void MecanumControl::drive(float x, float y, float turn) {
  calculateMotorPowers(x, y, turn);

  // Set motor directions and DAC outputs
  setMotor(leftFrontDirPin, leftFrontPower, MCP4728_CHANNEL_A);
  setMotor(rightFrontDirPin, rightFrontPower, MCP4728_CHANNEL_B);
  setMotor(leftRearDirPin, leftRearPower, MCP4728_CHANNEL_C);
  setMotor(rightRearDirPin, rightRearPower, MCP4728_CHANNEL_D);
}

// Getter methods for motor power values
float MecanumControl::getLeftFront() const { return leftFrontPower; }
float MecanumControl::getRightFront() const { return rightFrontPower; }
float MecanumControl::getLeftRear() const { return leftRearPower; }
float MecanumControl::getRightRear() const { return rightRearPower; }

// Calculate motor powers based on joystick inputs
void MecanumControl::calculateMotorPowers(float x, float y, float turn) {
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
void MecanumControl::setMotor(int directionPin, float motorValue, MCP4728_channel_t channel) {
  // Scale [-1, 1] to [0, 4095]
  int dacValue = abs(motorValue * 4095);
  dacValue = constrain(dacValue, 0, 4095);

  // Set direction: HIGH = CCW, LOW = CW
  digitalWrite(directionPin, motorValue >= 0 ? LOW : HIGH);

  // Set the corresponding MCP4728 channel output
  dac.setChannelValue(channel, dacValue);
}