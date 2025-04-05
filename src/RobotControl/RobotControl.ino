#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_MCP4728.h>
#include "XboxController.h"
#include "MecanumControl.h"
#include "DebugMenu.h"

// Motor driver pins for direction
const int leftFrontDirPin = 33;
const int rightFrontDirPin = 32;
const int leftRearDirPin = 30;
const int rightRearDirPin = 31;

// Motor Enable pins
const int leftFrontEnablePin = 42;
const int rightFrontEnablePin = 43;
const int leftRearEnablePin = 44;
const int rightRearEnablePin = 45;

bool printingEnabled = false; // Flag for serial printing

unsigned long lastPrintTime = 0; // Tracks the last time values were printed
const unsigned long printInterval = 1000; // Interval for printing in milliseconds (1 second)

// Initialize USB object
USB Usb;

// Create instances of XboxController and MecanumControl
XboxController xbox(&Usb);
MecanumControl drive;

// Initialize MCP4728 DAC
Adafruit_MCP4728 dac;

// Function to set motor direction and output analog voltage
void setMotor(int directionPin, float motorValue, MCP4728_channel_t channel) {
  
  // Scale [-1, 1] to [0, 4095]
  int dacValue = abs(motorValue * 4095);
  dacValue = constrain(dacValue, 0, 4095);

  // Set direction: HIGH = CCW, LOW = CW
  digitalWrite(directionPin, motorValue >= 0 ? LOW : HIGH);

  // Set the corresponding MCP4728 channel output
  dac.setChannelValue(channel, dacValue);
}

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial connection

  if (Usb.Init() == -1) {
    Serial.println(F("USB initialization failed!"));
    while (1);
  }
  Serial.println(F("XBOX USB Library Started"));

  // Wait for the Xbox center button to be held for 5 seconds
  Serial.println(F("Hold the Xbox center button for 5 seconds to start..."));
  waitForXboxCenterButton();

  // Initialize MCP4728 DAC
  if (!dac.begin()) {
    Serial.println(F("Failed to initialize MCP4728!"));
    while (1);
  }

  // Set Analog Output to 0
  dac.setChannelValue(MCP4728_CHANNEL_A, 0);
  dac.setChannelValue(MCP4728_CHANNEL_B, 0);
  dac.setChannelValue(MCP4728_CHANNEL_C, 0);
  dac.setChannelValue(MCP4728_CHANNEL_D, 0);

  // Set direction pins as outputs
  pinMode(leftFrontDirPin, OUTPUT);
  pinMode(rightFrontDirPin, OUTPUT);
  pinMode(leftRearDirPin, OUTPUT);
  pinMode(rightRearDirPin, OUTPUT);

  // Set enable pins as outputs
  pinMode(leftFrontEnablePin, OUTPUT);
  pinMode(rightFrontEnablePin, OUTPUT);
  pinMode(leftRearEnablePin, OUTPUT);
  pinMode (rightRearEnablePin, OUTPUT);

  // Set Enable pins LOW
  digitalWrite(leftFrontEnablePin, LOW);
  digitalWrite(rightFrontEnablePin, LOW);
  digitalWrite(leftRearEnablePin, LOW);
  digitalWrite(rightRearEnablePin, LOW);

  Serial.println(F("Setup complete"));
}

// Function to wait for the Xbox center button to be held for 5 seconds
void waitForXboxCenterButton() {
  unsigned long buttonPressStart = 0;
  bool buttonHeld = false;

  while (true) {
    Usb.Task(); // Process USB tasks

    if (xbox.isConnected()) {
      if (xbox.getStartButtonPressed()) { // Check if the center button is pressed
        if (!buttonHeld) {
          buttonPressStart = millis(); // Start timing when the button is first pressed
          buttonHeld = true;
        } else if (millis() - buttonPressStart >= 5000) { // Check if held for 5 seconds
          Serial.println(F("Xbox center button held for 5 seconds. Starting program..."));
          break;
        }
      } else {
        buttonHeld = false; // Reset if the button is released
      }
    }
  }
}

bool deadManActivated() {
  return xbox.getLT() == 255;
}

void loop() {
  Usb.Task();

  // Check for serial input to enter debug mode
  if (Serial.available()) {
    char command = Serial.read();
    if (command == 'd') {
      enterDebugMode(drive, xbox);
    } else if (command == 'p') {
      printingEnabled = !printingEnabled; // Toggle printingEnabled flag
      if (printingEnabled) {
        Serial.println(F("Normal mode serial printing enabled"));
      } else {
        Serial.println(F("Normal mode serial printing disabled"));
      }
    }
  }

  float x, y, turn;

  // Check if Xbox controller is connected
  if (xbox.isConnected() && deadManActivated()) {
    xbox.update();

    if (printingEnabled && millis() - lastPrintTime >= printInterval) {
      lastPrintTime = millis(); // Update the last print time
      Serial.print("DM pressed: ");
      Serial.print(xbox.getLT());
      Serial.println();
    }

    // Enable motors
    digitalWrite(leftFrontEnablePin, HIGH);
    digitalWrite(rightFrontEnablePin, HIGH);
    digitalWrite(leftRearEnablePin, HIGH);
    digitalWrite(rightRearEnablePin, HIGH);

    // Update controls
    x = xbox.getX();
    y = xbox.getY();
    turn = xbox.getTurn();

    // Calculate motor powers based on joystick inputs
    drive.calculateMotorPowers(x, y, turn);

    // Set motor directions and DAC outputs, invert the left motor direction
    setMotor(leftFrontDirPin, -drive.getLeftFront(), MCP4728_CHANNEL_A);
    setMotor(rightFrontDirPin, drive.getRightFront(), MCP4728_CHANNEL_B);
    setMotor(leftRearDirPin, -drive.getLeftRear(), MCP4728_CHANNEL_C);
    setMotor(rightRearDirPin, drive.getRightRear(), MCP4728_CHANNEL_D);

  } else {
    // Disable motors
    digitalWrite(leftFrontEnablePin, LOW);
    digitalWrite(rightFrontEnablePin, LOW);
    digitalWrite(leftRearEnablePin, LOW);
    digitalWrite(rightRearEnablePin, LOW);

    x = 0;
    y = 0;
    turn = 0;
  }

  // Serial output for debugging
  if (printingEnabled && millis() - lastPrintTime >= printInterval) {
    lastPrintTime = millis(); // Update the last print time
    Serial.print("LF: ");
    Serial.print(drive.getLeftFront(), 4);
    Serial.print("\tRF: ");
    Serial.print(drive.getRightFront(), 4);
    Serial.print("\tLR: ");
    Serial.print(drive.getLeftRear(), 4);
    Serial.print("\tRR: ");
    Serial.println(drive.getRightRear(), 4);
  }
}