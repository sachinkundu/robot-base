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
  pinMode(frontLeftDirPin, OUTPUT);
  pinMode(frontRightDirPin, OUTPUT);
  pinMode(rearLeftDirPin, OUTPUT);
  pinMode(rearRightDirPin, OUTPUT);

  // Set enable pins as outputs
  pinMode(frontLeftEnablePin, OUTPUT);
  pinMode(frontRightEnablePin, OUTPUT);
  pinMode(rearLeftEnablePin, OUTPUT);
  pinMode(rearRightEnablePin, OUTPUT);

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
  digitalWrite(frontLeftEnablePin, state ? HIGH : LOW);
  digitalWrite(frontRightEnablePin, state ? HIGH : LOW);
  digitalWrite(rearLeftEnablePin, state ? HIGH : LOW);
  digitalWrite(rearRightEnablePin, state ? HIGH : LOW);
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

void MecanumDrive::enableFrontLeftMotor() {
  digitalWrite(frontLeftEnablePin, HIGH);
}

void MecanumDrive::disableFrontLeftMotor() {
  digitalWrite(frontLeftEnablePin, LOW);
}

void MecanumDrive::enableFrontRightMotor() {
  digitalWrite(frontRightEnablePin, HIGH);
}

void MecanumDrive::disableFrontRightMotor() {
  digitalWrite(frontRightEnablePin, LOW);
}

void MecanumDrive::enableRearLeftMotor() {
  digitalWrite(rearLeftEnablePin, HIGH);
}

void MecanumDrive::disableRearLeftMotor() {
  digitalWrite(rearLeftEnablePin, LOW);
}

void MecanumDrive::enableRearRightMotor() {
  digitalWrite(rearRightEnablePin, HIGH);
}

void MecanumDrive::disableRearRightMotor() {
  digitalWrite(rearRightEnablePin, LOW);
}

// Calculate motor powers and set motor directions and DAC outputs
void MecanumDrive::move(float x, float y, float turn) {
  snapToCardinalDirection(x, y);

  // Apply ramp scaling to joystick inputs
  x = applyRamp(x);
  y = applyRamp(y);
  turn = applyRamp(turn);

  // Scale down strafing speed
  const float strafingScale = 0.5; // Adjust this value to control strafing speed (e.g., 0.5 for 50% speed)
  x *= strafingScale;

  calculateMotorPowers(x, y, turn);

  // Set motor directions and DAC outputs
  setMotor(frontLeftDirPin, frontLeftPower, MCP4728_CHANNEL_A);
  setMotor(frontRightDirPin, -frontRightPower, MCP4728_CHANNEL_B);
  setMotor(rearLeftDirPin, rearLeftPower, MCP4728_CHANNEL_C);
  setMotor(rearRightDirPin, -rearRightPower, MCP4728_CHANNEL_D);
}

// Apply ramp scaling to joystick input
float MecanumDrive::applyRamp(float input) {
  // Ramp factor controls the non-linearity (e.g., 1.5 for cubic scaling)
  const float rampFactor = 2.0;

  // Preserve the sign of the input
  return (input >= 0 ? 1 : -1) * pow(abs(input), rampFactor);
}

// Snap joystick input to cardinal or diagonal directions
void MecanumDrive::snapToCardinalDirection(float &x, float &y) {
  // Calculate the angle of the joystick input
  float angle = atan2(y, x) * 180 / PI; // Convert radians to degrees

  // Normalize the angle to the range [0, 360)
  if (angle < 0) {
    angle += 360;
  }

  // Snap to the nearest cardinal or diagonal direction
  if (angle >= 22.5 && angle < 67.5) { // Northeast
    x = 1;
    y = 1;
  } else if (angle >= 67.5 && angle < 112.5) { // North
    x = 0;
    y = 1;
  } else if (angle >= 112.5 && angle < 157.5) { // Northwest
    x = -1;
    y = 1;
  } else if (angle >= 157.5 && angle < 202.5) { // West
    x = -1;
    y = 0;
  } else if (angle >= 202.5 && angle < 247.5) { // Southwest
    x = -1;
    y = -1;
  } else if (angle >= 247.5 && angle < 292.5) { // South
    x = 0;
    y = -1;
  } else if (angle >= 292.5 && angle < 337.5) { // Southeast
    x = 1;
    y = -1;
  } else { // East
    x = 1;
    y = 0;
  }
}

// Getter methods for motor power values
float MecanumDrive::getFrontLeft() const { return frontLeftPower; }
float MecanumDrive::getFrontRight() const { return frontRightPower; }
float MecanumDrive::getRearLeft() const { return rearLeftPower; }
float MecanumDrive::getRearRight() const { return rearRightPower; }

// Calculate motor powers based on joystick inputs
void MecanumDrive::calculateMotorPowers(float x, float y, float turn) {
  frontLeftPower = y + x + turn;
  frontRightPower = y - x - turn;
  rearLeftPower = y - x + turn;
  rearRightPower = y + x - turn;

  // Normalize motor powers to the range [-1, 1]
  float maxPower = max(max(abs(frontLeftPower), abs(frontRightPower)),
                       max(abs(rearLeftPower), abs(rearRightPower)));
  if (maxPower > 1) {
    frontLeftPower /= maxPower;
    frontRightPower /= maxPower;
    rearLeftPower /= maxPower;
    rearRightPower /= maxPower;
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
  if (motor == "frontLeft") {
    directionPin = frontLeftDirPin;
    channel = MCP4728_CHANNEL_A;
  } else if (motor == "frontRight") {
    directionPin = frontRightDirPin;
    channel = MCP4728_CHANNEL_B;
  } else if (motor == "rearLeft") {
    directionPin = rearLeftDirPin;
    channel = MCP4728_CHANNEL_C;
  } else if (motor == "rearRight") {
    directionPin = rearRightDirPin;
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