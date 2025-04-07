#include <Arduino.h>
#include <SPI.h>
#include <HardwareSerial.h> // Ensure Serial is defined
#include "XboxController.h"
#include "MecanumDrive.h"
#include "DebugMenu.h"

bool printingEnabled = false; // Flag for serial printing

unsigned long lastPrintTime = 0; // Tracks the last time values were printed
const unsigned long printInterval = 1000; // Interval for printing in milliseconds (1 second)

USB Usb;

XboxController xbox(&Usb);
MecanumDrive mecanumDrive;

void setup() {
  Serial.begin(115200);
  while (!Serial); // Wait for serial connection

  if (Usb.Init() == -1) {
    Serial.println(F("USB initialization failed!"));
    while (1);
  }
  Serial.println(F("XBOX USB Library Started"));
}

bool waitForXboxSTARTButton() {
  unsigned long buttonPressStart = 0; // Tracks when the button was first pressed
  bool buttonHeld = false;           // Tracks if the button is currently held

  while (true) {
    Usb.Task(); // Process USB tasks

    if (xbox.isConnected()) {
      if (xbox.getStartButtonPressed()) { // Check if the start button is pressed
        if (!buttonHeld) {
          buttonPressStart = millis(); // Start the countdown
          buttonHeld = true;
        } else if (millis() - buttonPressStart >= 1000) { // Check if held for 5 seconds
          Serial.println(F("Xbox center button held for 5 seconds. Starting program..."));
          return true; // Exit and return success
        }
      } else {
        buttonHeld = false; // Reset if the button is released
      }
    }
  }
}

bool deadManActivated() {
  // Example implementation: Check if the left trigger (LT) is pressed
  return xbox.getLT() == 255; // Completely pressed.
}

void printMainMenu() {
  Serial.println(F("==================================="));
  Serial.println(F("           Main Menu               "));
  Serial.println(F("==================================="));
  Serial.println(F("Available Commands:"));
  Serial.println(F("  [d] Enter Debug Mode"));
  Serial.println(F("  [p] Toggle Serial Printing"));
  Serial.println(F("  [h] Display this Help Menu"));
  Serial.println(F("==================================="));
}

void loop() {
  static bool setupComplete = false; // Tracks whether the setup is complete

  // Wait for the Xbox center button to be held for 5 seconds
  if (!setupComplete) {
    Serial.println(F("Hold the Xbox center button for 5 seconds to start the program."));
    if (waitForXboxSTARTButton()) {
      // Initialize MecanumControl after the button is held
      if (!mecanumDrive.initialize()) {
        Serial.println(F("Failed to initialize MecanumControl!"));
        while (1);
      }

      Serial.println(F("Setup complete"));
      setupComplete = true; // Mark setup as complete
    }
    return; // Exit the loop until setup is complete
  }

  // Main program logic
  Usb.Task();

  // Display the main menu
  printMainMenu();

  // Check for serial input
  if (Serial.available()) {
    char command = Serial.read();
    if (command == 'd') {
      enterDebugMode(mecanumDrive, xbox);
    } else if (command == 'p') {
      printingEnabled = !printingEnabled; // Toggle printingEnabled flag
      if (printingEnabled) {
        Serial.println(F("Normal mode serial printing enabled"));
      } else {
        Serial.println(F("Normal mode serial printing disabled"));
      }
    } else if (command == 'h') {
      // Display the help menu again
      printMainMenu();
    } else {
      Serial.println(F("Invalid command. Type 'h' for help."));
    }
  }

  float x, y, turn;

  // If the controller is connected and the deadman switch is activated, update the motors
  if (xbox.isConnected() && deadManActivated()) {
    xbox.update();

    if (printingEnabled && millis() - lastPrintTime >= printInterval) {
      lastPrintTime = millis(); // Update the last print time
      Serial.print("DM pressed: ");
      Serial.print(xbox.getLT());
      Serial.println();
    }

    // Enable motors and drive
    mecanumDrive.enableMotors();
    x = xbox.getX();
    y = xbox.getY();
    turn = xbox.getTurn();
    mecanumDrive.move(x, y, turn);

  } else {
    // Disable motors
    mecanumDrive.disableMotors();
  }

  // Serial output for debugging
  if (printingEnabled && millis() - lastPrintTime >= printInterval) {
    lastPrintTime = millis(); // Update the last print time
    Serial.println(F("Motor powers updated"));
  }
}