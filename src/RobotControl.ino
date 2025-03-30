#include <SPI.h>
#include <Adafruit_MCP4728.h>
#include "XboxController.h"
#include "MecanumControl.h"

// Motor driver pins for direction
const int leftFrontDirPin = 2;
const int rightFrontDirPin = 4;
const int leftRearDirPin = 7;
const int rightRearDirPin = 8;

// Initialize USB object
USB Usb;

// Create instances of XboxController and MecanumControl
XboxController xbox(&Usb);
MecanumControl drive;

// Initialize MCP4728 DAC
Adafruit_MCP4728 mcp;

// Function to set motor direction and output analog voltage
void setMotor(int dirPin, float motorValue, uint8_t channel) {
  
  // Scale [-1, 1] to [0, 4095]
  int dacValue = abs(motorValue * 4095);
  dacValue = constrain(dacValue, 0, 4095);

  // Set direction: HIGH = CCW, LOW = CW
  digitalWrite(dirPin, motorValue >= 0 ? LOW : HIGH);

  // Set the corresponding MCP4728 channel output
  mcp.setChannelValue(channel, dacValue);
}

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial connection

  if (Usb.Init() == -1) {
    Serial.println(F("USB initialization failed!"));
    while (1);
  }
  Serial.println(F("XBOX USB Library Started"));

  // Initialize MCP4728 DAC
  if (!mcp.begin()) {
    Serial.println(F("Failed to initialize MCP4728!"));
    while (1);
  }

  // Set direction pins as outputs
  pinMode(leftFrontDirPin, OUTPUT);
  pinMode(rightFrontDirPin, OUTPUT);
  pinMode(leftRearDirPin, OUTPUT);
  pinMode(rightRearDirPin, OUTPUT);

  Serial.println(F("Setup complete"));
}

void loop() {
  Usb.Task();

  // Check if Xbox controller is connected
  if (xbox.isConnected()) {
    xbox.update();

    // Initialize joystick values
    float x = 0;
    float y = 0;
    float turn = 0;

    // If LT is pressed fully (255), get joystick inputs
    if (xbox.getLT() == 255) {
      x = xbox.getX();
      y = xbox.getY();
      turn = xbox.getTurn();
    }

    // Calculate motor powers based on joystick inputs (zero if LT is not pressed)
    drive.calculateMotorPowers(x, y, turn);

    // Set motor directions and DAC outputs, invert the left motor direction
    setMotor(leftFrontDirPin, -drive.getLeftFront(), MCP4728_CHANNEL_A);
    setMotor(rightFrontDirPin, drive.getRightFront(), MCP4728_CHANNEL_B);
    setMotor(leftRearDirPin, -drive.getLeftRear(), MCP4728_CHANNEL_C);
    setMotor(rightRearDirPin, drive.getRightRear(), MCP4728_CHANNEL_D);
  }
  
  /*
  //Serial output for debugging
  Serial.print("LF: ");
  Serial.print(drive.getLeftFront(), 4);
  Serial.print("\tRF: ");
  Serial.print(drive.getRightFront(), 4);
  Serial.print("\tLR: ");
  Serial.print(drive.getLeftRear(), 4);
  Serial.print("\tRR: ");
  Serial.println(drive.getRightRear(), 4);
  */
}