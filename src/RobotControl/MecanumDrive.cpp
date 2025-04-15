#include "MecanumDrive.h"

// Constructor
MecanumDrive::MecanumDrive() : motorsEnabled(false) {}

// Initialize the DAC and motor pins
bool MecanumDrive::initialize() {
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
void MecanumDrive::resetDACOutputs() {
  dac.setChannelValue(MCP4728_CHANNEL_A, 0);
  dac.setChannelValue(MCP4728_CHANNEL_B, 0);
  dac.setChannelValue(MCP4728_CHANNEL_C, 0);
  dac.setChannelValue(MCP4728_CHANNEL_D, 0);
}

// Helper method to set the state of all motor enable pins
void MecanumDrive::setMotorEnableState(bool state) {
  digitalWrite(leftFrontEnablePin, state ? HIGH : LOW);
  digitalWrite(rightFrontEnablePin, state ? HIGH : LOW);
  digitalWrite(leftRearEnablePin, state ? HIGH : LOW);
  digitalWrite(rightRearEnablePin, state ? HIGH : LOW);
}

// Enable motors
void MecanumDrive::enableMotors() {
  setMotorEnableState(true);
  motorsEnabled = true; // Update the state
}

// Disable motors
void MecanumDrive::disableMotors() {
  this->resetDACOutputs(); // Reset DAC outputs to 0
  setMotorEnableState(false);
  motorsEnabled = false; // Update the state
}

// Enable and disable functions for individual motors

void MecanumDrive::enableLeftFrontMotor() {
  digitalWrite(leftFrontEnablePin, HIGH);
}

void MecanumDrive::disableLeftFrontMotor() {
  digitalWrite(leftFrontEnablePin, LOW);
}

void MecanumDrive::enableRightFrontMotor() {
  digitalWrite(rightFrontEnablePin, HIGH);
}

void MecanumDrive::disableRightFrontMotor() {
  digitalWrite(rightFrontEnablePin, LOW);
}

void MecanumDrive::enableLeftRearMotor() {
  digitalWrite(leftRearEnablePin, HIGH);
}

void MecanumDrive::disableLeftRearMotor() {
  digitalWrite(leftRearEnablePin, LOW);
}

void MecanumDrive::enableRightRearMotor() {
  digitalWrite(rightRearEnablePin, HIGH);
}

void MecanumDrive::disableRightRearMotor() {
  digitalWrite(rightRearEnablePin, LOW);
}

// Calculate motor powers and set motor directions and DAC outputs
void MecanumDrive::move(float x, float y, float turn) {
  calculateMotorPowers(x, y, turn);

  // Set motor directions and DAC outputs
  setMotor(leftFrontDirPin, -leftFrontPower, MCP4728_CHANNEL_A);
  setMotor(rightFrontDirPin, rightFrontPower, MCP4728_CHANNEL_B);
  setMotor(leftRearDirPin, -leftRearPower, MCP4728_CHANNEL_C);
  setMotor(rightRearDirPin, rightRearPower, MCP4728_CHANNEL_D);
}

// Getter methods for motor power values
float MecanumDrive::getLeftFront() const { return leftFrontPower; }
float MecanumDrive::getRightFront() const { return rightFrontPower; }
float MecanumDrive::getLeftRear() const { return leftRearPower; }
float MecanumDrive::getRightRear() const { return rightRearPower; }

// Calculate motor powers based on joystick inputs
void MecanumDrive::calculateMotorPowers(float x, float y, float turn) {
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
void MecanumDrive::setMotor(int directionPin, float motorValue, MCP4728_channel_t channel) {
  // Scale [-1, 1] to [0, 4095]
  int dacValue = abs(motorValue * 4095);
  dacValue = constrain(dacValue, 0, 4095);

  // Set direction: HIGH = CCW, LOW = CW
  digitalWrite(directionPin, motorValue >= 0 ? LOW : HIGH);

  // Set the corresponding MCP4728 channel output
  dac.setChannelValue(channel, dacValue);
}

// Set motor RPM and direction
void MecanumDrive::setMotorRPM(const String &motor, int rpm, bool forward) {
  int directionPin;
  MCP4728_channel_t channel;

  // Map motor names to pins and DAC channels
  if (motor == "leftFront") {
    directionPin = leftFrontDirPin;
    channel = MCP4728_CHANNEL_A;
  } else if (motor == "rightFront") {
    directionPin = rightFrontDirPin;
    channel = MCP4728_CHANNEL_B;
  } else if (motor == "leftRear") {
    directionPin = leftRearDirPin;
    channel = MCP4728_CHANNEL_C;
  } else if (motor == "rightRear") {
    directionPin = rightRearDirPin;
    channel = MCP4728_CHANNEL_D;
  } else {
    Serial.println(F("Invalid motor name."));
    return;
  }

  // Clip RPM between 0 and maxRPM
  rpm = constrain(rpm, 0, maxRPM);

  // Normalize RPM to a value between 0 and 1
  float normalizedValue = static_cast<float>(rpm) / maxRPM;

  // Adjust direction based on the 'forward' flag
  normalizedValue = forward ? normalizedValue : -normalizedValue;

  // Debug output
  Serial.print(F("Motor: "));
  Serial.print(motor);
  Serial.print(F(", RPM: "));
  Serial.print(rpm);
  Serial.print(F(", Normalized Value: "));
  Serial.println(normalizedValue);

  // Use setMotor to handle direction and DAC output
  setMotor(directionPin, normalizedValue, channel);
}

// Set the maximum RPM
void MecanumDrive::setMaxRPM(int maxRPM) {
  this->maxRPM = maxRPM;
  Serial.print(F("Maximum RPM set to: "));
  Serial.println(maxRPM);
}

// Get the maximum RPM
int MecanumDrive::getMaxRPM() const {
  return maxRPM;
}

// Check if motors are enabled
bool MecanumDrive::areMotorsEnabled() {
  return motorsEnabled; // Return the current state of the motors
}