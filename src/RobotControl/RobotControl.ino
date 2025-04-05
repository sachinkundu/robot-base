#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_MCP4728.h>
#include "XboxController.h"
#include "MecanumControl.h"
#include "DebugMenu.h"

bool printingEnabled = false; // Flag for serial printing

unsigned long lastPrintTime = 0; // Tracks the last time values were printed
const unsigned long printInterval = 1000; // Interval for printing in milliseconds (1 second)

// Initialize USB object
USB Usb;

// Create instances of XboxController and MecanumControl
XboxController xbox(&Usb);
// Create an instance of MecanumControl with motor pins
MecanumControl drive(33, 32, 30, 31, 42, 43, 44, 45);

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

  // Initialize MecanumControl
  if (!drive.initialize()) {
    Serial.println(F("Failed to initialize MecanumControl!"));
    while (1);
  }

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

    // Enable motors and drive
    drive.enableMotors();
    x = xbox.getX();
    y = xbox.getY();
    turn = xbox.getTurn();
    drive.drive(x, y, turn);

  } else {
    // Disable motors
    drive.disableMotors();
  }

  // Serial output for debugging
  if (printingEnabled && millis() - lastPrintTime >= printInterval) {
    lastPrintTime = millis(); // Update the last print time
    Serial.println(F("Motor powers updated"));
  }
}